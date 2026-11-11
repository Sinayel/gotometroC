#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_LIGNE_14 = 1,
    SCREEN_ID_LIGNE_13 = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *ligne_14;
    lv_obj_t *ligne_13;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *lbl_metro1_line14;
    lv_obj_t *lbl_metro2_line14;
    lv_obj_t *lbl_metro_txt_line14;
    lv_obj_t *msg_perturbation_line14;
    lv_obj_t *alerte_logo;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *liege;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *lbl_metro1_line13;
    lv_obj_t *lbl_metro2_line13;
    lv_obj_t *lbl_metro_txt_line13;
    lv_obj_t *msg_perturbation_line_13;
    lv_obj_t *alerte_logo_1;
} objects_t;

extern objects_t objects;

void create_screen_ligne_14();
void tick_screen_ligne_14();

void create_screen_ligne_13();
void tick_screen_ligne_13();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/