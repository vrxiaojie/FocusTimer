#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: btn_style
lv_style_t *get_style_btn_style_MAIN_DEFAULT();
lv_style_t *get_style_btn_style_MAIN_FOCUSED();
lv_style_t *get_style_btn_style_MAIN_PRESSED();
void add_style_btn_style(lv_obj_t *obj);
void remove_style_btn_style(lv_obj_t *obj);

// Style: checkbox_style
lv_style_t *get_style_checkbox_style_MAIN_DEFAULT();
lv_style_t *get_style_checkbox_style_MAIN_FOCUS_KEY();
lv_style_t *get_style_checkbox_style_INDICATOR_DEFAULT();
lv_style_t *get_style_checkbox_style_INDICATOR_CHECKED();
void add_style_checkbox_style(lv_obj_t *obj);
void remove_style_checkbox_style(lv_obj_t *obj);

// Style: screen_style_dark
lv_style_t *get_style_screen_style_dark_MAIN_DEFAULT();
void add_style_screen_style_dark(lv_obj_t *obj);
void remove_style_screen_style_dark(lv_obj_t *obj);

// Style: label_style
lv_style_t *get_style_label_style_MAIN_DEFAULT();
void add_style_label_style(lv_obj_t *obj);
void remove_style_label_style(lv_obj_t *obj);

// Style: btn_label_style
lv_style_t *get_style_btn_label_style_MAIN_DEFAULT();
void add_style_btn_label_style(lv_obj_t *obj);
void remove_style_btn_label_style(lv_obj_t *obj);

// Style: progress_bar_style
lv_style_t *get_style_progress_bar_style_MAIN_DEFAULT();
lv_style_t *get_style_progress_bar_style_INDICATOR_DEFAULT();
lv_style_t *get_style_progress_bar_style_KNOB_DEFAULT();
void add_style_progress_bar_style(lv_obj_t *obj);
void remove_style_progress_bar_style(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/