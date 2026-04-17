#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_main_scr_next_page_btn(lv_event_t * e);
extern void action_submain_scr_prev_page_btn(lv_event_t * e);
extern void action_main_scr(lv_event_t * e);
extern void action_back_to_sub_main_btn(lv_event_t * e);
extern void action_submain_scr_enter_pomodoro_btn(lv_event_t * e);
extern void action_submain_scr_enter_mp3_btn(lv_event_t * e);
extern void action_submain_scr_enter_setting_btn(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/