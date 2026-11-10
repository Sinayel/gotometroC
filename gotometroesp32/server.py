#!/usr/bin/env python3
import json, os, urllib.request
from http.server import BaseHTTPRequestHandler, HTTPServer

API_KEY = os.environ["PRIM_KEY"]
TOKEN   = os.environ["RELAY_TOKEN"]
URL = ("https://prim.iledefrance-mobilites.fr/marketplace/v2/navitia/"
       "stop_areas/stop_area%3AIDFM%3A71370/lines/line%3AIDFM%3AC01384/"
       "routes/route%3AIDFM%3Astif%3Alocal-251106-C01384-5/departures"
       "?data_freshness=realtime&count=4")

class Relay(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != f"/m14?t={TOKEN}":
            self.send_response(403)
            self.end_headers()
            return
        try:
            req = urllib.request.Request(URL, headers={"apikey": API_KEY})
            data = json.load(urllib.request.urlopen(req, timeout=10))

            # heures : "20261110T190021" -> "19:00"
            heures = []
            for d in data.get("departures", []):
                t = d["stop_date_time"]["departure_date_time"]
                heures.append(f"{t[9:11]}:{t[11:13]}")

            # perturbations : message court du canal "titre"
            infos = []
            for d in data.get("disruptions", []):
                if d.get("status") != "active":
                    continue
                for m in d.get("messages", []):
                    if m.get("channel", {}).get("name") == "titre":
                        infos.append(m["text"])
                        break
            info = " | ".join(infos) if infos else "Trafic normal"

            body = " ".join(heures) + "\n" + info
            self.send_response(200)
        except Exception as e:
            body = f"err {e}"
            self.send_response(502)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.end_headers()
        self.wfile.write(body.encode("utf-8"))

HTTPServer(("0.0.0.0", 8000), Relay).serve_forever()