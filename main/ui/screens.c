#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    lv_obj_add_event_cb(obj, action_main_scr, LV_EVENT_SCREEN_LOADED, (void *)0);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // main_scr_goto_submain_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.main_scr_goto_submain_btn = obj;
            lv_obj_set_pos(obj, 350, 64);
            lv_obj_set_size(obj, 30, 40);
            lv_obj_add_event_cb(obj, action_goto_submain_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, ">");
                }
            }
        }
        {
            // main_scr_time_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_time_label = obj;
            lv_obj_set_pos(obj, 81, 31);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_condensed_3_100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_letter_space(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "12:00");
        }
        {
            // main_scr_temp_value_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_temp_value_label = obj;
            lv_obj_set_pos(obj, 59, 136);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "00.0℃");
        }
        {
            // main_scr_humid_value_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_humid_value_label = obj;
            lv_obj_set_pos(obj, 156, 136);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "00.0%");
        }
        {
            // main_scr_co2_value_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_co2_value_label = obj;
            lv_obj_set_pos(obj, 255, 136);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0000ppm");
        }
        {
            // main_scr_date_value_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_date_value_label = obj;
            lv_obj_set_pos(obj, 101, 6);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "10月15日");
        }
        {
            // main_scr_dayofweek_value_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_dayofweek_value_label = obj;
            lv_obj_set_pos(obj, 216, 6);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "星期一");
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_sub_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.sub_main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // submain_back_to_main_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.submain_back_to_main_btn = obj;
            lv_obj_set_pos(obj, 4, 64);
            lv_obj_set_size(obj, 30, 40);
            lv_obj_add_event_cb(obj, action_back_to_main_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -1, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "<");
                }
            }
        }
        {
            // submain_scr_enter_pomodoro_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.submain_scr_enter_pomodoro_btn = obj;
            lv_obj_set_pos(obj, 59, 24);
            lv_obj_set_size(obj, 77, 40);
            lv_obj_add_event_cb(obj, action_submain_scr_enter_pomodoro_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -9, -4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_label_set_text(obj, "番茄钟");
                }
            }
        }
        {
            // submain_scr_enter_setting_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.submain_scr_enter_setting_btn = obj;
            lv_obj_set_pos(obj, 183, 24);
            lv_obj_set_size(obj, 77, 40);
            lv_obj_add_event_cb(obj, action_submain_scr_enter_setting_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 1, -4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_label_set_text(obj, "设置");
                }
            }
        }
        {
            // submain_scr_enter_mp3_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.submain_scr_enter_mp3_btn = obj;
            lv_obj_set_pos(obj, 59, 104);
            lv_obj_set_size(obj, 77, 40);
            lv_obj_add_event_cb(obj, action_submain_scr_enter_mp3_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 1, -4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_label_set_text(obj, "MP3");
                }
            }
        }
    }
    
    tick_screen_sub_main();
}

void tick_screen_sub_main() {
}

void create_screen_mp3() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.mp3 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    lv_obj_add_event_cb(obj, action_back_to_main_btn, LV_EVENT_PRESSED, (void *)0);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // mp3_scr_back_to_main_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.mp3_scr_back_to_main_btn = obj;
            lv_obj_set_pos(obj, 6, 5);
            lv_obj_set_size(obj, 38, 29);
            lv_obj_add_event_cb(obj, action_back_to_main_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -1, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "<-");
                }
            }
        }
    }
    
    tick_screen_mp3();
}

void tick_screen_mp3() {
}

void create_screen_setting() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.setting = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    lv_obj_add_event_cb(obj, action_back_to_main_btn, LV_EVENT_PRESSED, (void *)0);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // setting_scr_back_to_main_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.setting_scr_back_to_main_btn = obj;
            lv_obj_set_pos(obj, 11, 11);
            lv_obj_set_size(obj, 38, 29);
            lv_obj_add_event_cb(obj, action_back_to_main_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -1, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "<-");
                }
            }
        }
    }
    
    tick_screen_setting();
}

void tick_screen_setting() {
}

void create_screen_pomodoro() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pomodoro = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    lv_obj_add_event_cb(obj, action_pomodoro_scr, LV_EVENT_SCREEN_LOAD_START, (void *)0);
    lv_obj_add_event_cb(obj, action_pomodoro_scr, LV_EVENT_SCREEN_UNLOAD_START, (void *)0);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // pomodoro_back_to_main_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.pomodoro_back_to_main_btn = obj;
            lv_obj_set_pos(obj, 6, 5);
            lv_obj_set_size(obj, 38, 29);
            lv_obj_add_event_cb(obj, action_back_to_main_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -1, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "<-");
                }
            }
        }
        {
            // pomodoro_scr_start_pause_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.pomodoro_scr_start_pause_btn = obj;
            lv_obj_set_pos(obj, 333, 14);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_pomodoro_scr_start_pause_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -13, -4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_custom_symbol_fa_pro_regular_26, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // pomodoro_scr_reset_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.pomodoro_scr_reset_btn = obj;
            lv_obj_set_pos(obj, 333, 64);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_pomodoro_scr_reset_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -11, -4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_custom_symbol_fa_pro_regular_26, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // pomodoro_scr_skip_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.pomodoro_scr_skip_btn = obj;
            lv_obj_set_pos(obj, 333, 114);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_pomodoro_scr_skip_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -10, -4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_custom_symbol_fa_pro_regular_26, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // pomodoro_scr_arc_progress
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.pomodoro_scr_arc_progress = obj;
            lv_obj_set_pos(obj, 112, 3);
            lv_obj_set_size(obj, 160, 160);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 0);
            lv_arc_set_bg_end_angle(obj, 360);
            lv_arc_set_rotation(obj, -90);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_arc(obj);
        }
        {
            lv_obj_t *obj = lv_arc_create(parent_obj);
            lv_obj_set_pos(obj, 111, 2);
            lv_obj_set_size(obj, 162, 162);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 0);
            lv_arc_set_bg_end_angle(obj, 360);
            lv_arc_set_rotation(obj, -90);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_arc(obj);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_arc_create(parent_obj);
            lv_obj_set_pos(obj, 115, 6);
            lv_obj_set_size(obj, 154, 154);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 0);
            lv_arc_set_bg_end_angle(obj, 360);
            lv_arc_set_rotation(obj, -90);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_arc(obj);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            // pomodoro_scr_timer_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pomodoro_scr_timer_label = obj;
            lv_obj_set_pos(obj, 146, 55);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_condensed_3_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_letter_space(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "25:00");
        }
        {
            // pomodoro_scr_working_status_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pomodoro_scr_working_status_label = obj;
            lv_obj_set_pos(obj, 160, 102);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_letter_space(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "专注中");
        }
        {
            // pomodoro_scr_nowtime_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pomodoro_scr_nowtime_label = obj;
            lv_obj_set_pos(obj, 6, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "00:00");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 4, 84);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_custom_symbol_fa_pro_regular_26, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 6, 48);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_custom_symbol_fa_pro_regular_26, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "");
        }
        {
            // pomodoro_scr_nap_cnt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pomodoro_scr_nap_cnt = obj;
            lv_obj_set_pos(obj, 33, 50);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_label_set_text(obj, "00");
        }
        {
            // pomodoro_scr_focus_cnt
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pomodoro_scr_focus_cnt = obj;
            lv_obj_set_pos(obj, 33, 86);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_label_set_text(obj, "00");
        }
    }
    
    tick_screen_pomodoro();
}

void tick_screen_pomodoro() {
}

void create_screen_message() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.message = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // message_scr_content_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.message_scr_content_label = obj;
            lv_obj_set_pos(obj, 82, 33);
            lv_obj_set_size(obj, 221, 80);
            add_style_label_style(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "content1\ncontent2\ncontent3");
        }
        {
            // message_scr_title_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.message_scr_title_label = obj;
            lv_obj_set_pos(obj, 97, 5);
            lv_obj_set_size(obj, 191, 23);
            add_style_label_style(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "title");
        }
        {
            // message_scr_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.message_scr_btn = obj;
            lv_obj_set_pos(obj, 143, 120);
            lv_obj_set_size(obj, 100, 35);
            lv_obj_add_event_cb(obj, action_message_scr_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_btn_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_btn_label_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "OK");
                }
            }
        }
    }
    
    tick_screen_message();
}

void tick_screen_message() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_sub_main,
    tick_screen_mp3,
    tick_screen_setting,
    tick_screen_pomodoro,
    tick_screen_message,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
    { "siyuanheiti_20", &ui_font_siyuanheiti_20 },
    { "Roboto-Condensed-3_100", &ui_font_roboto_condensed_3_100 },
    { "Roboto-Condensed-3_40", &ui_font_roboto_condensed_3_40 },
    { "custom_symbol_solid_26", &ui_font_custom_symbol_solid_26 },
    { "custom_symbol_fa-pro_regular_26", &ui_font_custom_symbol_fa_pro_regular_26 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_main();
    create_screen_sub_main();
    create_screen_mp3();
    create_screen_setting();
    create_screen_pomodoro();
    create_screen_message();
}