#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_btn2_action(lv_event_t * e);
extern void action_btn1_action(lv_event_t * e);
extern void action_btn3_action(lv_event_t * e);
extern void action_btn4_action(lv_event_t * e);
extern void action_btn5_action(lv_event_t * e);
extern void action_btn6_action(lv_event_t * e);
extern void action_timer_screen_action(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/