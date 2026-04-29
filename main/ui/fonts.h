#ifndef EEZ_LVGL_UI_FONTS_H
#define EEZ_LVGL_UI_FONTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_font_t ui_font_siyuanheiti_20;
extern const lv_font_t ui_font_roboto_condensed_3_100;
extern const lv_font_t ui_font_roboto_condensed_3_40;
extern const lv_font_t ui_font_custom_symbol_solid_26;
extern const lv_font_t ui_font_custom_symbol_fa_pro_regular_26;
extern const lv_font_t ui_font_custom_symbol_fa_pro_regular_16;
extern const lv_font_t ui_font_siyuanheiti_16;
extern const lv_font_t ui_font_dyna_puff_medium;

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