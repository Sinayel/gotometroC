![Language](https://img.shields.io/badge/language-C/C++-blue) ![Tech](https://img.shields.io/badge/tech-ESP32-purple) ![License](https://img.shields.io/badge/license-MIT-purple)
# GoToMetro ESP32

Sert a afficher l'heure des prochain metro, la c'est de base configurer sur l'arret, Saint-Laz ligne 14 direction Orly, il donne les 4 prochains  passages.

pour la version [esp32](https://github.com/Sinayel/gotometroC/tree/main/gotometroesp32) commencer par :
| PC (libcurl) | ESP32 (Arduino)     |
| :-------- | :------- |
| `curl_easy_init()` | `HTTPClient http;` |
| `CURLOPT_URL` | `http.begin(client, url)` |
| `curl_slist_append("apikey: …")` | `http.addHeader("apikey", "…")` |
| `curl_easy_perform()` | `http.GET()` |
| `save + reponse[]` | `http.getString()` |
| `strstr / printf` | `identique (Serial.printf)` |



(1) HTTPClient http -> (2) http.begin(client, url) -> (3) http.addHeader("apikey", "…") -> (4) http.GET() -> (5) http.getString() -> (6) Serial.printf(s + 9, s + 13)

(j'ai aussi fais une [version en C](https://github.com/Sinayel/gotometroC/blob/main/srcs/gotometroEsp32.c))
## Documentation

[libcurl](https://curl.se/libcurl/)
\
[Navitia PRIM (Playground)](https://prim.iledefrance-mobilites.fr/playground/index.html)

