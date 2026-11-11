#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_13_logo;
extern const lv_img_dsc_t img_50x50_pixl_logo_14;
extern const lv_img_dsc_t img_45;
extern const lv_img_dsc_t img_50;
extern const lv_img_dsc_t img_metro_logo;
extern const lv_img_dsc_t img_50_png;
extern const lv_img_dsc_t img_m_logo35;
extern const lv_img_dsc_t img_14_logo_35pix;
extern const lv_img_dsc_t img_alerte_orange;
extern const lv_img_dsc_t img_alerte_gris;
extern const lv_img_dsc_t img_50_50_13;
extern const lv_img_dsc_t img_35x35_13;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[12];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/