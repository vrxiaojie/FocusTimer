#include <stddef.h>

#include "screens.h"
#include "message_screen_calls.h"

static lv_obj_t *s_previous_screen = NULL;
static message_screen_confirm_cb_t s_confirm_cb = NULL;
static void *s_confirm_cb_user_data = NULL;

static lv_obj_t *message_button_label(void)
{
	if (objects.message_scr_btn == NULL)
	{
		return NULL;
	}

	return lv_obj_get_child(objects.message_scr_btn, 0);
}

void message_screen_set_title(const char *title)
{
	if (objects.message_scr_title_label != NULL && title != NULL)
	{
		lv_label_set_text(objects.message_scr_title_label, title);
	}
}

void message_screen_set_content(const char *content)
{
	if (objects.message_scr_content_label != NULL && content != NULL)
	{
		lv_label_set_text(objects.message_scr_content_label, content);
	}
}

void message_screen_set_button_text(const char *button_text)
{
	lv_obj_t *btn_label = message_button_label();
	if (btn_label != NULL && button_text != NULL)
	{
		lv_label_set_text(btn_label, button_text);
	}
}

void message_screen_set_confirm_callback(message_screen_confirm_cb_t cb, void *user_data)
{
	s_confirm_cb = cb;
	s_confirm_cb_user_data = user_data;
}

void message_screen_show(void)
{
	lv_obj_t *current_screen = lv_screen_active();
	if (current_screen != objects.message)
	{
		s_previous_screen = current_screen;
	}

	if (objects.message != NULL)
	{
		lv_screen_load_anim(objects.message, LV_SCREEN_LOAD_ANIM_FADE_ON, 120, 0, false);
	}
}

void message_screen_show_with_text(const char *title, const char *content, const char *button_text)
{
	message_screen_set_title(title);
	message_screen_set_content(content);
	message_screen_set_button_text(button_text);
	message_screen_show();
}

void message_screen_handle_ok(void)
{
	if (s_confirm_cb != NULL)
	{
		s_confirm_cb(s_confirm_cb_user_data);
	}

	s_confirm_cb = NULL;
	s_confirm_cb_user_data = NULL;

	lv_obj_t *target = s_previous_screen;
	if (target == NULL || target == objects.message)
	{
		target = objects.main;
	}

	if (target != NULL)
	{
		lv_screen_load_anim(target, LV_SCREEN_LOAD_ANIM_FADE_ON, 120, 0, false);
	}

	s_previous_screen = NULL;
}
