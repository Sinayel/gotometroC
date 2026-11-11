#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "secret.h"


/* ---------- Ecran ---------- */
Arduino_DataBus *bus = new Arduino_ESP32QSPI(45, 47, 21, 48, 40, 39);
Arduino_GFX *gfx = new Arduino_NV3041A(bus, GFX_NOT_DEFINED, 0, true);

#define BUF_LINES 80
static uint16_t *buf;

/* ---------- Tactile résistif XPT2046 ---------- */
#define T_SCK  12
#define T_MISO 13
#define T_MOSI 11
#define T_CS   38
#define T_IRQ  3

#define TS_MINX 300
#define TS_MAXX 3800
#define TS_MINY 300
#define TS_MAXY 3800
#define CALIB_TOUCH 0   // mettre à 1 pour afficher les valeurs brutes dans le moniteur

SPIClass touchSPI(HSPI);
XPT2046_Touchscreen ts(T_CS, 255);   // 255 = pas d'IRQ

/* ---------- Réglages ---------- */
#define REFRESH_MS     30000   // interrogation du serveur (2 lignes = 2 appels PRIM)
#define AUTO_SWITCH_MS 0       // > 0 : change de page tout seul (ex. 10000), 0 = désactivé
#define ANIM_MS        450     // durée de l'animation de changement de page
#define INVERSE_SWIPE  0       // mettre à 1 si l'animation part dans le mauvais sens

struct Ligne {
  const char *url;
  lv_obj_t *page;
  lv_obj_t *metro1;
  lv_obj_t *metro2;
  lv_obj_t *clock;
  lv_obj_t *info;
  lv_obj_t *alerte;
};
static Ligne lignes[2];
static int page_actuelle = 0;

/* ---------- LVGL : affichage ---------- */
void my_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px) {
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px, w, h);
  lv_display_flush_ready(disp);
}

static uint32_t my_tick() { return millis(); }

/* ---------- LVGL : tactile ---------- */
#define SWIPE_MIN 700
static volatile bool swipe_demande = false;
static volatile bool swipe_gauche = false;

void my_touch(lv_indev_t *indev, lv_indev_data_t *data) {
  static bool appui = false;
  static int16_t x_debut, y_debut, x_fin, y_fin;

  if (ts.touched()) {
    TS_Point p = ts.getPoint();
#if CALIB_TOUCH
    Serial.printf("x=%d y=%d\n", p.x, p.y);
#endif
    if (!appui) {
      appui = true;
      x_debut = p.x;
      y_debut = p.y;
    }
    x_fin = p.x;
    y_fin = p.y;
    data->point.x = constrain(map(p.x, TS_MINX, TS_MAXX, 0, 479), 0, 479);
    data->point.y = constrain(map(p.y, TS_MINY, TS_MAXY, 0, 271), 0, 271);
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    if (appui) {
      appui = false;
      int dx = x_fin - x_debut;
      int dy = y_fin - y_debut;
      if (abs(dx) > SWIPE_MIN || abs(dy) > SWIPE_MIN) {
        swipe_demande = true;
        // on regarde l'axe qui a le plus bougé, puis son signe
        bool gauche = (abs(dx) > abs(dy)) ? (dx < 0) : (dy < 0);
        swipe_gauche = INVERSE_SWIPE ? !gauche : gauche;
      }
    }
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

/* ---------- Pages ---------- */
static void montre_page(int index, lv_screen_load_anim_t anim) {
  if (index == page_actuelle) return;
  page_actuelle = index;
  lv_screen_load_anim(lignes[index].page, anim, ANIM_MS, 0, false);
}

// Rend tous les enfants non scrollables et fait remonter les gestes jusqu'à la page
static void prepare_gestes(lv_obj_t *obj) {
  lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
  for (uint32_t i = 0; i < lv_obj_get_child_count(obj); i++)
    prepare_gestes(lv_obj_get_child(obj, i));
}

/* ---------- Data ---------- */
static String mot(const String &s, int n) {
  int debut = 0;
  for (int i = 0; i < n; i++) {
    debut = s.indexOf(' ', debut);
    if (debut < 0) return "";
    debut++;
  }
  int fin = s.indexOf(' ', debut);
  return fin < 0 ? s.substring(debut) : s.substring(debut, fin);
}

static void affiche_minutes(lv_obj_t *lbl, const String &m) {
  if (m.length() == 0)
    lv_label_set_text(lbl, "--");
  else
    lv_label_set_text_fmt(lbl, "%s", m.c_str());
}

static void affiche_info(const Ligne &l, const String &info, bool erreur) {
  lv_label_set_text(l.info, info.c_str());
  bool normal = !erreur && info == "Trafic normal";
  lv_obj_set_style_text_color(l.info, lv_color_hex(normal ? 0x2E7D32 : 0xC62828), 0);
  if (normal)
    lv_obj_add_flag(l.alerte, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_remove_flag(l.alerte, LV_OBJ_FLAG_HIDDEN);
}

void fetch_ligne(const Ligne &l) {
  HTTPClient http;
  http.setTimeout(5000);
  http.begin(l.url);
  int code = http.GET();

  if (code == 200) {
    String rep = http.getString();            // "1 5 8 12\nPanne d'un ascenseur\n..."
    int nl = rep.indexOf('\n');
    String minutes = nl >= 0 ? rep.substring(0, nl) : rep;
    String info    = nl >= 0 ? rep.substring(nl + 1) : "";

    affiche_minutes(l.metro1, mot(minutes, 0));
    affiche_minutes(l.metro2, mot(minutes, 1));
    affiche_info(l, info, false);
  } else {
    lv_label_set_text_fmt(l.metro1, "Err %d", code);
    lv_label_set_text(l.metro2, "--");
    affiche_info(l, "Serveur injoignable", true);
  }
  http.end();
}

/* ---------- Setup ---------- */
void setup() {
  Serial.begin(115200);

  delay(1000);
  Serial.println("Demarrage OK");

  gfx->begin();
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);                      // rétroéclairage

  lv_init();
  lv_tick_set_cb(my_tick);

  lv_display_t *disp = lv_display_create(gfx->width(), gfx->height());
  size_t size = gfx->width() * BUF_LINES * sizeof(uint16_t);
  buf = (uint16_t *)heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  lv_display_set_buffers(disp, buf, NULL, size, LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, my_flush);

  touchSPI.begin(T_SCK, T_MISO, T_MOSI, T_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touch);

  ui_init();

  lignes[0] = { API_URL_14, objects.ligne_14, objects.lbl_metro1_line14, objects.lbl_metro2_line14,
                objects.lbl_metro_txt_line14, objects.msg_perturbation_line14, objects.alerte_logo };
  lignes[1] = { API_URL_13, objects.ligne_13, objects.lbl_metro1_line13, objects.lbl_metro2_line13,
                objects.lbl_metro_txt_line13, objects.msg_perturbation_line_13, objects.alerte_logo };

  for (int i = 0; i < 2; i++) {
    prepare_gestes(lignes[i].page);
    lv_label_set_text(lignes[i].metro1, "WiFi...");
    lv_label_set_text(lignes[i].metro2, "");
    lv_label_set_text(lignes[i].info, "");
    lv_obj_add_flag(lignes[i].alerte, LV_OBJ_FLAG_HIDDEN);
  }
  lv_screen_load(lignes[0].page);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

/* ---------- Loop ---------- */
void loop() {
  lv_timer_handler();
  ui_tick();

  if (swipe_demande) {
    swipe_demande = false;
    int cible = 1 - page_actuelle;    // 0 -> 1, 1 -> 0
    montre_page(cible, swipe_gauche ? LV_SCR_LOAD_ANIM_OVER_LEFT : LV_SCR_LOAD_ANIM_OVER_RIGHT);
  }

  static unsigned long last_fetch = 0;
  static bool first = true;
  if (WiFi.status() == WL_CONNECTED && (first || millis() - last_fetch > REFRESH_MS)) {
    if (first)
      configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");   // heure de Paris
    first = false;
    last_fetch = millis();
    for (int i = 0; i < 2; i++)
      fetch_ligne(lignes[i]);
  }

  static unsigned long last_clock = 0;
  if (millis() - last_clock > 1000) {
    last_clock = millis();
    struct tm t;
    if (getLocalTime(&t, 0))
      for (int i = 0; i < 2; i++)
        lv_label_set_text_fmt(lignes[i].clock, "%02d:%02d", t.tm_hour, t.tm_min);
  }

#if AUTO_SWITCH_MS > 0
  static unsigned long last_switch = 0;
  if (millis() - last_switch > AUTO_SWITCH_MS) {
    last_switch = millis();
    montre_page(1 - page_actuelle,
                page_actuelle == 0 ? LV_SCR_LOAD_ANIM_OVER_LEFT : LV_SCR_LOAD_ANIM_OVER_RIGHT);
  }
#endif

  delay(5);
}
