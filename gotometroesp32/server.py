#!/usr/bin/env python3
import json, os, urllib.request
from http.server import BaseHTTPRequestHandler, HTTPServer

API_KEY = os.environ["PRIM_KEY"]
TOKEN   = os.environ["RELAY_TOKEN"]
URL = ("https://prim.iledefrance-mobilites.fr/marketplace/v2/navitia/"
       "stop_areas/stop_area%3AIDFM%3A71370/lines/line%3AIDFM%3AC01384/"
       "routes/route%3AIDFM%3Astif%3Alocal-251106-C01384-5/departures"
       "?data_freshness=realtime&count=4&depth=0")

class Relay(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != f"/m14?t={TOKEN}":
            self.send_response(403);
            self.end_headers();
            return
        try:
            req = urllib.request.Request(URL, headers={"apikey": API_KEY})  # fais la requete GET sur l'URL demander
            data = json.load(urllib.request.urlopen(req, timeout=10))   # Seconde requete pour l'avoir en json clean
            print(json.dumps(data, indent=2, ensure_ascii=False))

            times = []
            for d in data["departures"]:    # va au debut de la ligne des departs 
                times.append(d["stop_date_time"]["departure_date_time"])
            # print(times)
            body = " ".join(f"{t[9:11]}:{t[11:13]}" for t in times)    # relie tous les caractere ensemble avec : de separation a partir de la ligne de depart 'times'
            self.send_response(200)     # retourne 200 comme code ok
        except Exception as e:
            body = f"err {e}"
            self.send_response(502) # retourne 502 en code erreur
        self.end_headers()  # clean
        self.wfile.write(body.encode())

HTTPServer(("0.0.0.0", 8000), Relay).serve_forever()    # run sur le localhost au port 8000