#!/usr/bin/env python3
import json, os, urllib.request
from datetime import datetime
from zoneinfo import ZoneInfo
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
            req = urllib.request.Request(URL, headers={"apikey": API_KEY})   # prépare
            with urllib.request.urlopen(req, timeout=10) as resp:           # ouvre
                data = json.load(resp) 

            now = datetime.now(ZoneInfo("Europe/Paris")).replace(tzinfo=None)
            minutes = []
            for d in data.get("departures", []):
                t = datetime.strptime(d["stop_date_time"]["departure_date_time"], "%Y%m%dT%H%M%S")
                m = int((t - now).total_seconds() // 60)
                minutes.append(str(max(m, 0)))   # 0 si le métro est a quai

            infos = []
            for d in data.get("disruptions", []):
                if d.get("status") != "active":
                    continue
                for m in d.get("messages", []):
                    if m.get("channel", {}).get("name") == "sms ascenseurs":
                        infos.append(m["text"])
                        print(infos)
                        break

            if infos:
                info = " | ".join(infos)
            else:
                "Aucune perturbation !"

            body = " ".join(minutes) + "\n" + info
            self.send_response(200)
        except Exception as e:
            body = f"err {e}"
            self.send_response(502)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.end_headers()
        self.wfile.write(body.encode("utf-8"))

HTTPServer(("0.0.0.0", 8000), Relay).serve_forever()