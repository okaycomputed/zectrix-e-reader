#include "pages/screen_saver.h"

#include "lcd_display.h"
#include "lvgl_theme.h"
#include "board.h"

#include <esp_log.h>

#include <cstdio>
#include <cstring>
#include <ctime>

namespace {
constexpr lv_coord_t kPageWidth = 400;
constexpr lv_coord_t kPageHeight = 300;
constexpr char kTag[] = "ScreenSaver";
} //namespace

ScreenSaver::ScreenSaver(LcdDisplay* host) : host_(host) {}

ScreenSaver::~ScreenSaver() {

}

UiPageId ScreenSaver::Id() const {
    return UiPageId::ScreenSaver;
}

const char* ScreenSaver::Name() const {
    return "ScreenSaver";
}

const lv_font_t* ScreenSaver::CurrentFont() const {
    if (host_ == nullptr) {
        return nullptr;
    }
    auto* theme = static_cast<LvglTheme*>(host_->GetTheme());
    if (theme == nullptr || theme->text_font() == nullptr) {
        return nullptr;
    }
    return theme->text_font()->font();
}

void ScreenSaver::Build() {
    if (screen_ != nullptr) {
        return;
    }

    const lv_font_t* font = CurrentFont();

    screen_ = lv_obj_create(nullptr);
    lv_obj_set_size(screen_, kPageWidth, kPageHeight);
    lv_obj_set_style_bg_color(screen_, lv_color_white(), 0);
    lv_obj_set_style_border_width(screen_, 0, 0);
    lv_obj_set_style_pad_all(screen_, 0, 0);

    LoadScreensaverImage();

    // Screensaver text
    screen_saver_text_ = lv_label_create(screen_);
    if (font != nullptr) {
        lv_obj_set_style_text_font(screen_saver_text_, font, 0);
    }
    lv_obj_set_style_text_color(screen_saver_text_, lv_color_black(), 0);
    lv_label_set_text(screen_saver_text_, "Press any button to wake");
    
    // Aligns the text below the image
    lv_obj_align(screen_saver_text_, LV_ALIGN_CENTER, 0, 90);

}

lv_obj_t* ScreenSaver::Screen() const {
    return screen_;
}

void ScreenSaver::OnShow() {
    
}

void ScreenSaver::OnHide() {
    
}  

// Screen wakes and goes back to the home screen when any button is clicked
bool ScreenSaver::HandleEvent(const UiPageEvent& event) {
    switch (event.type) {
        case UiPageEventType::ButtonUp:
        case UiPageEventType::ButtonDown:
        case UiPageEventType::ButtonConfirm:
            if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::Home)) {
                host_->RequestUrgentFullRefresh();
            }
            return true;

        default:
            return false;
    }
}

void ScreenSaver::LoadScreensaverImage() {
    LV_IMAGE_DECLARE(ReadingNudaeng);
    
    nudaeng_img_ = lv_image_create(screen_);
    
    // Set the source image
    lv_image_set_src(nudaeng_img_, &ReadingNudaeng);
    
    // Put in the middle of the page
    lv_obj_align(nudaeng_img_, LV_ALIGN_CENTER, 0, -20);

    // Scale it down
    lv_image_set_scale(nudaeng_img_, 80);
}
