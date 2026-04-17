#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SUB_MAIN = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *sub_main;
    lv_obj_t *main_scr_next_page_btn;
    lv_obj_t *main_scr_time_hour_label;
    lv_obj_t *main_scr_time_minute_label;
    lv_obj_t *main_scr_time_minute_label_1;
    lv_obj_t *main_scr_temp_value_label;
    lv_obj_t *main_scr_humid_value_label;
    lv_obj_t *main_scr_humid_value_label_1;
    lv_obj_t *main_scr_date_value_label;
    lv_obj_t *main_scr_dayofweek_value_label;
    lv_obj_t *submain_scr_prev_page_btn;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_sub_main();
void tick_screen_sub_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/