#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char *ssid = "SFRRRR";
const char *pass = "e194v5q7clm2j1q559az";
const char *url  = "https://prim.iledefrance-mobilites.fr/marketplace/v2/navitia/"
                   "stop_areas/stop_area%3AIDFM%3A71370/lines/line%3AIDFM%3AC01384/"
                   "routes/route%3AIDFM%3Astif%3Alocal-251106-C01384-5/departures"
                   "?data_freshnes s=realtime&count=4&depth=0";

void setup()
{
    Serial
}