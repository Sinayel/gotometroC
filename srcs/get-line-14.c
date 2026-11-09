#include "../include/header.h"

char reponse[100000];
size_t taille = 0;

// Save le json de la requete
size_t save(char *data, size_t size, size_t n, void *userdata)
{
    (void)userdata;
    memcpy(reponse + taille, data, size * n);
    taille += size * n;
    reponse[taille] = '\0';

    return (size * n);
}

int main(void)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers, "apikey: m2SM6QIcYsPNgsR2lNUH3Cy6mWyQTDzC");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, "https://prim.iledefrance-mobilites.fr/marketplace/v2/navitia/stop_areas/stop_area%3AIDFM%3A71370/lines/line%3AIDFM%3AC01384/routes/route%3AIDFM%3Astif%3Alocal-251106-C01384-5/departures?data_freshness=realtime&count=4&");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save);

    curl_easy_perform(curl);

    const char *depart_str = "\"departure_date_time\":\"";
    char *s = reponse;
    int i = 0;

    while ((s = strstr(s, depart_str)) != NULL) {
        s += strlen(depart_str);    // On saute le departure data time
        printf("--------------------------------------------\n");
        printf("  %d - Metro (14) > Aéroport d'Orly : %.2s:%.2s\n", ++i, s + 9, s + 11);    // s + 9 = 1er nb | s + 11 = 2eme nb
        printf("--------------------------------------------\n");
    }

    curl_easy_cleanup(curl);

    return 0;
}