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
            // main_scr_next_page_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.main_scr_next_page_btn = obj;
            lv_obj_set_pos(obj, 350, 64);
            lv_obj_set_size(obj, 30, 40);
            lv_obj_add_event_cb(obj, action_main_scr_next_page_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
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
            // main_scr_time_hour_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_time_hour_label = obj;
            lv_obj_set_pos(obj, 76, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_condensed_3_100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "12");
        }
        {
            // main_scr_time_minute_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_time_minute_label = obj;
            lv_obj_set_pos(obj, 197, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_condensed_3_100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "00");
        }
        {
            // main_scr_time_minute_label_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_time_minute_label_1 = obj;
            lv_obj_set_pos(obj, 174, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_label_style(obj);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_condensed_3_100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, ":");
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
            // main_scr_humid_value_label_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_scr_humid_value_label_1 = obj;
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
            // submain_scr_prev_page_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.submain_scr_prev_page_btn = obj;
            lv_obj_set_pos(obj, 4, 64);
            lv_obj_set_size(obj, 30, 40);
            lv_obj_add_event_cb(obj, action_submain_scr_prev_page_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
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

void create_screen_pomodoro() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.pomodoro = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // pomodoro_scr_prev_page_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.pomodoro_scr_prev_page_btn = obj;
            lv_obj_set_pos(obj, 11, 11);
            lv_obj_set_size(obj, 38, 29);
            lv_obj_add_event_cb(obj, action_back_to_sub_main_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
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
    
    tick_screen_pomodoro();
}

void tick_screen_pomodoro() {
}

void create_screen_mp3() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.mp3 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 384, 168);
    lv_obj_add_event_cb(obj, action_back_to_sub_main_btn, LV_EVENT_PRESSED, (void *)0);
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // mp3_scr_prev_page_btn
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.mp3_scr_prev_page_btn = obj;
            lv_obj_set_pos(obj, 11, 11);
            lv_obj_set_size(obj, 38, 29);
            lv_obj_add_event_cb(obj, action_back_to_sub_main_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
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
    add_style_screen_style_dark(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // mp3_scr_prev_page_btn_1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.mp3_scr_prev_page_btn_1 = obj;
            lv_obj_set_pos(obj, 11, 11);
            lv_obj_set_size(obj, 38, 29);
            lv_obj_add_event_cb(obj, action_back_to_sub_main_btn, LV_EVENT_SHORT_CLICKED, (void *)0);
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

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_sub_main,
    tick_screen_pomodoro,
    tick_screen_mp3,
    tick_screen_setting,
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
    create_screen_pomodoro();
    create_screen_mp3();
    create_screen_setting();
}