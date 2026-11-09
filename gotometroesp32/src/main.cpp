#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>


const char *ssid = "SFRRRR";
const char *pass = "e194v5q7clm2j1q559az";
const char *url  = "https://prim.iledefrance-mobilites.fr/marketplace/v2/navitia/"
                   "stop_areas/stop_area%3AIDFM%3A71370/lines/line%3AIDFM%3AC01384/"
                   "routes/route%3AIDFM%3Astif%3Alocal-251106-C01384-5/departures"
                   "?data_freshnes s=realtime&count=4&depth=0";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
}

void loop() {
  WiFiClientSecure client;
  client.setInsecure();               // HTTPS sans verifier le certificat

  HTTPClient http;
  http.begin(client, url);
  http.addHeader("apikey", "TA_CLE");

  if (http.GET() == 200) {
      String rep = http.getString();
      const char *key = "\"departure_date_time\":\"";
      const char *s = rep.c_str();
      while ((s = strstr(s, key))) {
          s += strlen(key);
          Serial.printf("M14 : %.2s:%.2s\n", s + 9, s + 11);
      }
  }
  http.end();
  delay(30000); 
}
