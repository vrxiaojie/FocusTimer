#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_goto_submain_btn(lv_event_t * e);
extern void action_back_to_main_btn(lv_event_t * e);
extern void action_main_scr(lv_event_t * e);
extern void action_submain_scr_enter_pomodoro_btn(lv_event_t * e);
extern void action_submain_scr_enter_mp3_btn(lv_event_t * e);
extern void action_submain_scr_enter_setting_btn(lv_event_t * e);
extern void action_pomodoro_scr(lv_event_t * e);
extern void action_pomodoro_scr_start_pause_btn(lv_event_t * e);
extern void action_pomodoro_scr_reset_btn(lv_event_t * e);
extern void action_pomodoro_scr_skip_btn(lv_event_t * e);
extern void action_message_scr_btn(lv_event_t * e);
extern void action_mp3_scr(lv_event_t * e);
extern void action_mp3_scr_backward_btn(lv_event_t * e);
extern void action_mp3_scr_play_pause_btn(lv_event_t * e);
extern void action_mp3_scr_forward_btn(lv_event_t * e);
extern void action_mp3_scr_volume_slider(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/