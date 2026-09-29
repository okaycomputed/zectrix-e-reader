#include "pages/user_settings.h"

#include "lcd_display.h"
#include "lvgl_theme.h"
#include "board.h"

#include <esp_log.h>

namespace {
constexpr lv_coord_t kPageWidth = 400;
constexpr lv_coord_t kPageHeight = 300;
constexpr char kTag[] = "UserSettings";
} //namespace

UserSettings::UserSettings(LcdDisplay* host) : host_(host) {}

UserSettings::~UserSettings() {
    if (header != nullptr) {
        page_header_delete(header);
        header = nullptr;
    }
}

UiPageId UserSettings::Id() const {
    return UiPageId::UserSettings;
}

const char* UserSettings::Name() const {
    return "Settings";
}

const lv_font_t* UserSettings::CurrentFont() const {
    if (host_ == nullptr) {
        return nullptr;
    }

    auto* theme = static_cast<LvglTheme*>(host_->GetTheme());
    if (theme == nullptr || theme->text_font() == nullptr) {
        return nullptr;
    }

    return theme->text_font()->font();
}

void UserSettings::Build() {
    if (screen_ != nullptr) {
        return;
    }

    // Setting page font
    const lv_font_t* font = CurrentFont();

    // Screen LVGL object
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_size(screen_, kPageWidth, kPageHeight);
    lv_obj_set_style_bg_color(screen_, lv_color_white(), 0);
    lv_obj_set_style_border_width(screen_, 0, 0);
    lv_obj_set_style_pad_all(screen_, 0, 0);

    // Create the page header widget
    header = page_header_create(screen_, Name(), this, font);

    // Create placeholder text
    placeholder_label_ = lv_label_create(screen_);
    lv_label_set_text(placeholder_label_, "No settings avaliable");
    lv_obj_set_style_text_color(placeholder_label_, lv_color_black(), 0);
    lv_obj_align(placeholder_label_, LV_ALIGN_TOP_LEFT, 0, 28);

    if(font) {
        lv_obj_set_style_text_font(placeholder_label_, font, 0);
    }

}


lv_obj_t* UserSettings::Screen() const {
    return screen_;
}

void UserSettings::OnShow() {
    
}

void UserSettings::OnHide() {
    
}

bool UserSettings::HandleEvent(const UiPageEvent& event) {
    switch(event.type){
        case UiPageEventType::ButtonConfirmLongPress:
            if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::ScreenSaver)) {
                host_->RequestUrgentFullRefresh();
            }
            return true; 
        
        default:
            return false;
    }
}