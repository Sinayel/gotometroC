#ifndef EEZ_LVGL_UI_FONTS_H
#define EEZ_LVGL_UI_FONTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_font_t ui_font_paris_65;
extern const lv_font_t ui_font_parisine_30;
extern const lv_font_t ui_font_parisine_55;
extern const lv_font_t ui_font_parisine_40;
extern const lv_font_t ui_font_parisine_15;
extern const lv_font_t ui_font_parisine_23;

#ifndef EXT_FONT_DESC_T
#define EXT_FONT_DESC_T
typedef struct _ext_font_desc_t {
    const char *name;
    const void *font_ptr;
} ext_font_desc_t;
#endif

extern ext_font_desc_t fonts[];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_FONTS_H*/