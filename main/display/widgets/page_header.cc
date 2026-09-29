#include "page_header.h"

namespace {
    constexpr lv_coord_t kPageWidth = 400;
    constexpr lv_coord_t kPageHeight = 300;
    constexpr lv_coord_t kPageHeaderHeight = 28;
} // namespace

page_header_t *page_header_create(lv_obj_t *parent, const char *page_title, void *user_data, const lv_font_t *font)
{
    auto *header = static_cast<page_header_t *>(lv_malloc(sizeof(page_header_t)));
    header->user_data = user_data;

    // Creating the page header object inside the parent defined in the parameter (the screen of the specified page)
    header->root = lv_obj_create(parent);

    // Make root container ignore input
    lv_obj_clear_flag(header->root, LV_OBJ_FLAG_CLICKABLE);

    // Setting the size of the page header
    lv_obj_set_size(header->root, kPageWidth, kPageHeaderHeight);

    // Styling the page header
    lv_obj_set_pos(header->root, 0, 0);
    lv_obj_set_style_bg_color(header->root, lv_color_white(), 0);
    lv_obj_set_style_border_side(header->root, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(header->root, 1, 0);
    lv_obj_set_style_border_color(header->root, lv_color_black(), 0);
    lv_obj_set_style_pad_all(header->root, 0, 0);
    lv_obj_set_style_pad_hor(header->root, 8, 0);

    // Creates the back button
    header->back_button = lv_btn_create(header->root);

    // Styling the back button
    lv_obj_set_size(header->back_button, 66, 24);
    lv_obj_set_style_bg_color(header->back_button, lv_color_white(), 0);
    lv_obj_align(header->back_button, LV_ALIGN_LEFT_MID, 0, 0);

    // Arrow icon
    header->back_icon = lv_label_create(header->back_button);
    lv_label_set_text(header->back_icon, "←");
    lv_obj_set_style_text_color(header->back_icon, lv_color_black(), 0);
    lv_obj_align(header->back_icon, LV_ALIGN_LEFT_MID, -10, 0);

    // Add text to button
    header->button_text = lv_label_create(header->back_button);
    lv_label_set_text(header->button_text, "Back");
    lv_obj_set_style_text_color(header->button_text, lv_color_black(), 0);
    lv_obj_align(header->button_text, LV_ALIGN_LEFT_MID, 12, 0);

    // Creates the page title
    header->page_title = lv_label_create(header->root);
    lv_label_set_text(header->page_title, page_title);
    lv_obj_align(header->page_title, LV_ALIGN_CENTER, 0, 0);

    // Setting fonts
    if (font) {
        lv_obj_set_style_text_font(header->back_icon, font, 0);
        lv_obj_set_style_text_font(header->button_text, font, 0);
        lv_obj_set_style_text_font(header->page_title, font, 0);
    }

    return header;
}

void page_header_set_back_focused(page_header_t *header, bool focused) {
    if (header == nullptr || header->back_button == nullptr) {
        return;
    }

    // Styling button color when focused/not focused 
    lv_obj_set_style_bg_opa(header->back_button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(header->back_button, focused ? lv_color_black() : lv_color_white(), 0);
    lv_obj_set_style_border_color(header->back_button, focused ? lv_color_black() : lv_color_white(), 0);
    lv_obj_set_style_border_width(header->back_button, 1, 0);
    lv_obj_set_style_radius(header->back_button, 4, 0);
    
    // Changing icon and font color
    lv_obj_set_style_text_color(header->back_icon, focused ? lv_color_white() : lv_color_black(), 0);
    lv_obj_set_style_text_color(header->button_text, focused ? lv_color_white() : lv_color_black(), 0);
}

void page_header_delete(page_header_t *header) {
    lv_obj_delete(header->root);
    // Frees the structure 
    lv_free(header);
}