pour la version esp32 il faut commencer par :

PC (libcurl)	                ESP32 (Arduino)

curl_easy_init()	            HTTPClient http;
CURLOPT_URL	                    http.begin(client, url)
curl_slist_append("apikey: …")	http.addHeader("apikey", "…")
curl_easy_perform()	            http.GET()
save + reponse[]	            http.getString() (fait le buffer pour toi)
strstr / printf	                identique (Serial.printf)

(1) HTTPClient http -> (2) http.begin(client, url) -> (3) http.addHeader("apikey", "…") -> (4) http.GET() -> (5) http.getString() -> (6) Serial.printf(s + 9, s + 13)