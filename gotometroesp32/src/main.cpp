#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "ui/secret.h"

Arduino_DataBus *bus = new Arduino_ESP32QSPI(45, 47, 21, 48, 40, 39);
Arduino_GFX *gfx = new Arduino_NV3041A(bus, GFX_NOT_DEFINED, 0, true);

#define BUF_LINES 40
static uint16_t *buf;

// LVGL appelle ça pour envoyer une zone rendue vers l'écran
void my_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px) {
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px, w, h);
  lv_display_flush_ready(disp);
}

static uint32_t my_tick() { return millis(); }

void fetch_times() {
  HTTPClient http;
  http.begin(API_URL);
  int code = http.GET();

  if (code == 200) {
    String rep = http.getString();
    int nl = rep.indexOf('\n');
    String heures = nl >= 0 ? rep.substring(0, nl) : rep;
    String info   = nl >= 0 ? rep.substring(nl + 1) : "";

    if (heures.length() >= 5)
      lv_label_set_text(objects.lbl_metro1, heures.substring(3, 5).c_str());
    if (heures.length() >= 11)
      lv_label_set_text(objects.lbl_metro2, heures.substring(9, 11).c_str());
    lv_label_set_text(objects.msg_perturbation, info.c_str());
  } else {
    lv_label_set_text_fmt(objects.lbl_metro1, "Err %d", code);
    lv_label_set_text(objects.lbl_metro2, "--:--");
    lv_label_set_text(objects.msg_perturbation, "Serveur injoignable");
  }
  http.end();
}

void setup() {
  gfx->begin();
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);              // rétroéclairage

  lv_init();
  lv_tick_set_cb(my_tick);

  lv_display_t *disp = lv_display_create(gfx->width(), gfx->height());
  size_t size = gfx->width() * BUF_LINES * sizeof(uint16_t);
  buf = (uint16_t *)heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  lv_display_set_buffers(disp, buf, NULL, size, LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, my_flush);

  ui_init();
  lv_obj_clear_flag(objects.main, LV_OBJ_FLAG_SCROLLABLE);
  lv_label_set_text(objects.lbl_metro1, "WiFi...");
  lv_label_set_text(objects.lbl_metro2, "");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");  // heure de Paris
}

unsigned long last = 0;
bool first = true;

void loop() {
  lv_timer_handler();
  ui_tick();

  if (WiFi.status() == WL_CONNECTED && (first || millis() - last > 15000)) {
    first = false;
    last = millis();
    fetch_times();
  }
  // Horloge a droite
  static unsigned long lastClock = 0;
  if (millis() - lastClock > 1000) {
    lastClock = millis();
    struct tm t;
    if (getLocalTime(&t, 0))
      lv_label_set_text_fmt(objects.lbl_metro_txt, "%02d:%02d", t.tm_hour, t.tm_min);
  }
  delay(5);
}