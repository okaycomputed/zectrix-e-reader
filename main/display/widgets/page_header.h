#pragma once
#include "lvgl.h"

typedef struct {
    lv_obj_t *root;
    lv_obj_t *back_button;
    lv_obj_t *back_icon;
    lv_obj_t *button_text;
    lv_obj_t *page_title;
    const lv_font_t *font;
    void *user_data;

} page_header_t;

page_header_t *page_header_create(lv_obj_t *header, const char *page_title, void *user_data, const lv_font_t *font);
void page_header_delete(page_header_t *header);
void page_header_set_back_focused(page_header_t *header, bool focused);