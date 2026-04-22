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
    SCREEN_ID_MP3 = 3,
    SCREEN_ID_SETTING = 4,
    SCREEN_ID_POMODORO = 5,
    SCREEN_ID_MESSAGE = 6,
    _SCREEN_ID_LAST = 6
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *sub_main;
    lv_obj_t *mp3;
    lv_obj_t *setting;
    lv_obj_t *pomodoro;
    lv_obj_t *message;
    lv_obj_t *main_scr_goto_submain_btn;
    lv_obj_t *main_scr_time_label;
    lv_obj_t *main_scr_temp_value_label;
    lv_obj_t *main_scr_humid_value_label;
    lv_obj_t *main_scr_co2_value_label;
    lv_obj_t *main_scr_date_value_label;
    lv_obj_t *main_scr_dayofweek_value_label;
    lv_obj_t *submain_back_to_main_btn;
    lv_obj_t *submain_scr_enter_pomodoro_btn;
    lv_obj_t *submain_scr_enter_setting_btn;
    lv_obj_t *submain_scr_enter_mp3_btn;
    lv_obj_t *mp3_scr_back_to_main_btn;
    lv_obj_t *mp3_scr_musictitle_label;
    lv_obj_t *mp3_scr_play_progress;
    lv_obj_t *mp3_scr_backward_btn;
    lv_obj_t *mp3_scr_play_pause_btn;
    lv_obj_t *mp3_scr_forward_btn;
    lv_obj_t *mp3_scr_volume_slider;
    lv_obj_t *mp3_scr_current_total_time_label;
    lv_obj_t *mp3_scr_file_count_label;
    lv_obj_t *setting_scr_back_to_main_btn;
    lv_obj_t *pomodoro_back_to_main_btn;
    lv_obj_t *pomodoro_scr_start_pause_btn;
    lv_obj_t *pomodoro_scr_reset_btn;
    lv_obj_t *pomodoro_scr_skip_btn;
    lv_obj_t *pomodoro_scr_arc_progress;
    lv_obj_t *pomodoro_scr_timer_label;
    lv_obj_t *pomodoro_scr_working_status_label;
    lv_obj_t *pomodoro_scr_nowtime_label;
    lv_obj_t *pomodoro_scr_nap_cnt;
    lv_obj_t *pomodoro_scr_focus_cnt;
    lv_obj_t *message_scr_content_label;
    lv_obj_t *message_scr_title_label;
    lv_obj_t *message_scr_btn;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_sub_main();
void tick_screen_sub_main();

void create_screen_mp3();
void tick_screen_mp3();

void create_screen_setting();
void tick_screen_setting();

void create_screen_pomodoro();
void tick_screen_pomodoro();

void create_screen_message();
void tick_screen_message();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/