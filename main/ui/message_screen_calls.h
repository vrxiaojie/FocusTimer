#pragma once

typedef void (*message_screen_confirm_cb_t)(void *user_data);

void message_screen_set_title(const char *title);
void message_screen_set_content(const char *content);
void message_screen_set_button_text(const char *button_text);
void message_screen_set_confirm_callback(message_screen_confirm_cb_t cb, void *user_data);
void message_screen_show(void);
void message_screen_show_with_text(const char *title, const char *content, const char *button_text);
void message_screen_handle_ok(void);
