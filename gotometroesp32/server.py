#!/usr/bin/env python3
"""Relais HTTP entre l'API PRIM (TLS 1.3 uniquement) et l'afficheur ESP32 GoToMetro.

Réponse texte : "<min1> <min2> ...\n<info perturbation>"
"""
import json
import os
import urllib.error
import urllib.request
from datetime import datetime
from http.server import BaseHTTPRequestHandler, HTTPServer
from zoneinfo import ZoneInfo

API_KEY = os.environ["PRIM_KEY"]
TOKEN = os.environ["RELAY_TOKEN"]
PARIS = ZoneInfo("Europe/Paris")

BASE = "https://prim.iledefrance-mobilites.fr/marketplace/v2/navitia/"
PARAMS = "/departures?data_freshness=realtime&count=4"


def url_ligne(stop_area, ligne, route):
    """Construit l'URL PRIM des prochains départs (les ':' deviennent %3A)."""
    return (BASE
            + "stop_areas/" + stop_area.replace(":", "%3A")
            + "/lines/" + ligne.replace(":", "%3A")
            + "/routes/" + route.replace(":", "%3A")
            + PARAMS)


# Une entrée par ligne : arrêt, ligne, route (= direction)
LIGNES = {
    # M14 à Saint-Lazare, direction Aéroport d'Orly
    "/m14": url_ligne("stop_area:IDFM:71370", "line:IDFM:C01384",
                      "route:IDFM:stif:local-251106-C01384-5"),
    # M13 à Liège, direction Châtillon - Montrouge
    "/m13": url_ligne("stop_area:IDFM:71382", "line:IDFM:C01383",
                      "route:IDFM:stif:local-251112-C01383-7"),
}


def appel_prim(url):
    """Interroge PRIM et renvoie la réponse JSON sous forme de dictionnaire."""
    req = urllib.request.Request(url, headers={"apikey": API_KEY})
    with urllib.request.urlopen(req, timeout=10) as resp:
        return json.load(resp)


def minutes_restantes(data):
    """Liste des minutes avant chaque départ, ex. ["1", "5", "8"]."""
    now = datetime.now(PARIS).replace(tzinfo=None)
    minutes = []
    for d in data.get("departures", []):
        t = datetime.strptime(d["stop_date_time"]["departure_date_time"], "%Y%m%dT%H%M%S")
        m = int((t - now).total_seconds() // 60)
        minutes.append(str(max(m, 0)))
    return minutes


def nettoie_detail(texte):
    """'GARE ST LAZARE Panne ... situé Niveau Rue / ...' -> 'Rue / ...'."""
    if "situé " in texte:
        return texte.split("situé ", 1)[1].replace("Niveau ", "")
    return texte.replace("GARE ST LAZARE ", "")


def perturbations(data):
    """Texte des perturbations actives (titre + détail), ou 'Trafic normal'."""
    infos = []
    for d in data.get("disruptions", []):
        if d.get("status") != "active":
            continue
        titre, detail = "", ""
        for m in d.get("messages", []):
            canal = m.get("channel", {}).get("name", "")
            if canal == "titre":
                titre = m["text"]
            elif canal == "moteur":
                detail = nettoie_detail(m["text"])
        if titre or detail:
            infos.append(f"{titre}\n{detail}".strip())
    return "\n\n".join(infos) if infos else "Trafic normal"


class Relay(BaseHTTPRequestHandler):
    """Répond à /m14?t=TOKEN et /m13?t=TOKEN."""

    def do_GET(self):  # pylint: disable=invalid-name
        """Traite une requête de l'ESP32."""
        chemin, _, query = self.path.partition("?")
        if query != f"t={TOKEN}" or chemin not in LIGNES:
            self.send_response(403)
            self.end_headers()
            return
        try:
            data = appel_prim(LIGNES[chemin])
            body = " ".join(minutes_restantes(data)) + "\n" + perturbations(data)
            self.send_response(200)
        except (urllib.error.URLError, TimeoutError, ValueError, KeyError) as e:
            body = f"err {e}"
            self.send_response(502)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.end_headers()
        self.wfile.write(body.encode("utf-8"))


HTTPServer(("0.0.0.0", 8000), Relay).serve_forever()
