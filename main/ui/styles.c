#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: btn_style
//

void init_style_btn_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_shadow_width(style, 0);
    lv_style_set_border_side(style, LV_BORDER_SIDE_FULL);
    lv_style_set_border_color(style, lv_color_hex(0xff000000));
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
    lv_style_set_border_width(style, 2);
};

lv_style_t *get_style_btn_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_btn_style_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_btn_style_MAIN_FOCUSED(lv_style_t *style) {
    lv_style_set_shadow_width(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_border_color(style, lv_color_hex(0xffffffff));
    lv_style_set_border_width(style, 2);
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_btn_style_MAIN_FOCUSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_btn_style_MAIN_FOCUSED(style);
    }
    return style;
};

void init_style_btn_style_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_border_width(style, 3);
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_border_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_btn_style_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_btn_style_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_btn_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_btn_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_btn_style_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_add_style(obj, get_style_btn_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_btn_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_btn_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_btn_style_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_remove_style(obj, get_style_btn_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: checkbox_style
//

void init_style_checkbox_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
};

lv_style_t *get_style_checkbox_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_checkbox_style_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_checkbox_style_MAIN_FOCUS_KEY(lv_style_t *style) {
    lv_style_set_outline_color(style, lv_color_hex(0xffffffff));
    lv_style_set_outline_width(style, 1);
    lv_style_set_outline_opa(style, 255);
};

lv_style_t *get_style_checkbox_style_MAIN_FOCUS_KEY() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_checkbox_style_MAIN_FOCUS_KEY(style);
    }
    return style;
};

void init_style_checkbox_style_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xffffffff));
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_checkbox_style_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_checkbox_style_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_checkbox_style_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_checkbox_style_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_checkbox_style_INDICATOR_CHECKED(style);
    }
    return style;
};

void add_style_checkbox_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_checkbox_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_checkbox_style_MAIN_FOCUS_KEY(), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_add_style(obj, get_style_checkbox_style_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_checkbox_style_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
};

void remove_style_checkbox_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_checkbox_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_checkbox_style_MAIN_FOCUS_KEY(), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(obj, get_style_checkbox_style_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_checkbox_style_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
};

//
// Style: screen_style_dark
//

void init_style_screen_style_dark_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_screen_style_dark_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_style_dark_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_style_dark(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_style_dark_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_style_dark(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_style_dark_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: label_style
//

void init_style_label_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_siyuanheiti_20);
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_label_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: btn_label_style
//

void init_style_btn_label_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_siyuanheiti_20);
};

lv_style_t *get_style_btn_label_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_btn_label_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_btn_label_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_btn_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_btn_label_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_btn_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_btn_style,
        add_style_checkbox_style,
        add_style_screen_style_dark,
        add_style_label_style,
        add_style_btn_label_style,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_btn_style,
        remove_style_checkbox_style,
        remove_style_screen_style_dark,
        remove_style_label_style,
        remove_style_btn_label_style,
    };
    remove_style_funcs[styleIndex](obj);
}