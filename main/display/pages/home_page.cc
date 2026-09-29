#include "pages/home_page.h"

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
constexpr lv_coord_t kStatusBarHeight = 28;
constexpr lv_coord_t kTabBarHeight = 32;
constexpr int kTabCount = 4;
constexpr lv_coord_t kTabWidth = 150;
constexpr char kTag[] = "HomePage";
constexpr uint32_t kStatusRefreshMs = 60 * 1000;  // re-read RTC/battery once a minute
}  // namespace

HomePage::HomePage(LcdDisplay* host) : host_(host) {}

HomePage::~HomePage() {
    if (status_timer_ != nullptr) {
        lv_timer_del(status_timer_);
        status_timer_ = nullptr;
    }
}

UiPageId HomePage::Id() const {
    return UiPageId::Home;
}

const char* HomePage::Name() const {
    return "Home";
}

const char* HomePage::TabName(Tab tab) {
    switch (tab) {
        case Tab::Library:   return "Library";
        case Tab::Bookmarks: return "Bookmarks";
        case Tab::Upload:    return "Upload";
        case Tab::Settings:  return "Settings";
        default:             return "?";
    }
}

const lv_font_t* HomePage::CurrentFont() const {
    if (host_ == nullptr) {
        return nullptr;
    }
    auto* theme = static_cast<LvglTheme*>(host_->GetTheme());
    if (theme == nullptr || theme->text_font() == nullptr) {
        return nullptr;
    }
    return theme->text_font()->font();
}

void HomePage::Build() {
    if (screen_ != nullptr) {
        return;
    }

    screen_ = lv_obj_create(nullptr);
    lv_obj_set_size(screen_, kPageWidth, kPageHeight);
    lv_obj_set_style_bg_color(screen_, lv_color_white(), 0);
    lv_obj_set_style_border_width(screen_, 0, 0);
    lv_obj_set_style_pad_all(screen_, 0, 0);

    BuildStatusBar();

    BuildTabBar();

    SelectTab(0);

    ESP_LOGI(kTag, "Home page built");
}

void HomePage::BuildStatusBar() {
    const lv_font_t* font = CurrentFont();

     // Creates the status bar above the home page
    status_bar_ = lv_obj_create(screen_);
    lv_obj_set_pos(status_bar_, 0, 0);
    lv_obj_set_size(status_bar_, kPageWidth, kStatusBarHeight);
    lv_obj_set_style_bg_color(status_bar_, lv_color_white(), 0);
    lv_obj_set_style_border_side(status_bar_, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(status_bar_, 1, 0);
    lv_obj_set_style_border_color(status_bar_, lv_color_black(), 0);
    lv_obj_set_style_pad_all(status_bar_, 0, 0);
    lv_obj_set_style_pad_hor(status_bar_, 8, 0);

    // Creates the date and time labels
    date_time_label_ = lv_label_create(status_bar_);
    if (font != nullptr) {
        lv_obj_set_style_text_font(date_time_label_, font, 0);
    }
    lv_obj_set_style_text_color(date_time_label_, lv_color_black(), 0);
    lv_label_set_text(date_time_label_, "--:--");
    lv_obj_align(date_time_label_, LV_ALIGN_LEFT_MID, 0, 0);

    // Creates the battery level indicator (displays in %)
    battery_label_ = lv_label_create(status_bar_);
    if (font != nullptr) {
        lv_obj_set_style_text_font(battery_label_, font, 0);
    }
    lv_obj_set_style_text_color(battery_label_, lv_color_black(), 0);
    lv_label_set_text(battery_label_, "--%");
    lv_obj_align(battery_label_, LV_ALIGN_RIGHT_MID, 0, 0);
}

// Builds the vertical tab-bar on the left side of the home page
void HomePage::BuildTabBar() {
    const lv_font_t* font = CurrentFont();

    // Sets the properties of the (parent) tab bar
    tab_bar_ = lv_obj_create(screen_);
    lv_obj_set_pos(tab_bar_, 0, kStatusBarHeight);
    lv_obj_set_size(tab_bar_, kTabWidth, kPageHeight);
    lv_obj_set_style_bg_color(tab_bar_, lv_color_white(), 0);
    lv_obj_set_style_pad_left(tab_bar_, 10, 0);

    // Flexes the tab boxes vertically
    lv_obj_set_layout(tab_bar_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(tab_bar_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tab_bar_,
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_START);

    // Uses a for-loop to initialize each tab (child)
    for (int i = 0; i < kTabCount; i++) {
        lv_obj_t* tab = lv_label_create(tab_bar_);
        if (font != nullptr) {
            lv_obj_set_style_text_font(tab, font, 0);
        }
        lv_label_set_long_mode(tab, LV_LABEL_LONG_CLIP);
        lv_obj_set_width(tab, lv_pct(100));
        lv_obj_set_height(tab, kTabBarHeight);
        lv_obj_set_style_text_align(tab, LV_TEXT_ALIGN_LEFT, 0);
        lv_label_set_text(tab, TabName(static_cast<Tab>(i)));
        lv_obj_set_style_pad_all(tab, 8, 0);
        lv_obj_set_style_bg_opa(tab, LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_color(tab, lv_color_black(), 0);
        tab_labels_[i] = tab;
    }
}

void HomePage::SelectTab(int index) {
    if (kTabCount == 0) {
        return;
    }

    index = ((index % kTabCount) + kTabCount) % kTabCount;  // wrap both directions
    selected_tab_ = index;

    for (int i = 0; i < kTabCount; i++) {
        lv_obj_t* tab = tab_labels_[i];
        if (tab == nullptr) {
            continue;
        }
        const bool selected = (i == selected_tab_);
        lv_obj_set_style_bg_opa(tab, selected ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_color(tab, lv_color_black(), 0);
        lv_obj_set_style_border_color(tab, lv_color_black(), 0);
        lv_obj_set_style_radius(tab, 10, 0);
        lv_obj_set_style_text_color(tab, selected ? lv_color_white() : lv_color_black(), 0);
    }
}

void HomePage::RefreshStatusBar() {
    if (date_time_label_ != nullptr) {
        tm now_tm{};
        char buf[32];
        if (Board::GetInstance().GetLocalTime(now_tm)) {
            strftime(buf, sizeof(buf), "%a %b %d  %H:%M", &now_tm);
        }

        else {
            std::snprintf(buf, sizeof(buf), "--:--");
        }

        lv_label_set_text(date_time_label_, buf);
    }

    // TODO: Battery detection is inconsistent and requires some fixing to preserve accuracy 
    if (battery_label_ != nullptr) {
        int level = 0;
        bool charging = false;
        bool discharging = false;
        char buf[16];

        if (Board::GetInstance().GetBatteryLevel(level, charging, discharging)) {
            std::snprintf(buf, sizeof(buf), charging ? "Charging" : "%d%%", level);
        }

        else {
            std::snprintf(buf, sizeof(buf), "--%%");
        }

        lv_label_set_text(battery_label_, buf);
    }
}

void HomePage::StatusTimerCallback(lv_timer_t* timer) {
    auto* self = static_cast<HomePage*>(lv_timer_get_user_data(timer));
    if (self == nullptr) {
        return;
    }

    self->RefreshStatusBar();

    if (self->host_ != nullptr) {
        self->host_->RequestUrgentRefresh();
    }
}

lv_obj_t* HomePage::Screen() const {
    return screen_;
}

void HomePage::OnShow() {
    if (status_timer_ == nullptr) {
        status_timer_ = lv_timer_create(StatusTimerCallback, kStatusRefreshMs, this);
    }
    RefreshStatusBar();
}

void HomePage::OnHide() {
    if (status_timer_ != nullptr) {
        lv_timer_del(status_timer_);
        status_timer_ = nullptr;
    }
}  

bool HomePage::HandleEvent(const UiPageEvent& event) {
    switch (event.type) {
        // Refreshes when switching through tabs
        case UiPageEventType::ButtonUp:
            SelectTab(selected_tab_ - 1);

            if (host_ != nullptr) {
                host_->RequestUrgentRefresh();
            }
            return true;

        case UiPageEventType::ButtonDown:
            SelectTab(selected_tab_ + 1);

            if (host_ != nullptr) {
                host_->RequestUrgentRefresh();
            }
            return true;
        
        // Redirects into their respective page
        case UiPageEventType::ButtonConfirm:
            ESP_LOGI(kTag, "Confirm on tab: %s", TabName(static_cast<Tab>(selected_tab_)));

            // Switches to the library page
            if(strcmp(TabName(static_cast<Tab>(selected_tab_)), "Library") == 0) {
                if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::Library)) {
                    host_->RequestUrgentFullRefresh();
                }
                return true;
            }

            // Switches to the bookmarks page
            if(strcmp(TabName(static_cast<Tab>(selected_tab_)), "Bookmarks") == 0) {
                if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::Bookmarks)) {
                    host_->RequestUrgentFullRefresh();
                }
                return true;
            }

            // Switches to the upload page
            if(strcmp(TabName(static_cast<Tab>(selected_tab_)), "Upload") == 0) {
                if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::Upload)) {
                    host_->RequestUrgentFullRefresh();
                }
                return true;
            }

            // Switches to the settings page
            if(strcmp(TabName(static_cast<Tab>(selected_tab_)), "Settings") == 0) {
                if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::UserSettings)) {
                    host_->RequestUrgentFullRefresh();
                }
                return true;
            }

            return true;
        
        // Forces device into its screensaver
        case UiPageEventType::ButtonConfirmLongPress:
            if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::ScreenSaver)) {
                host_->RequestUrgentFullRefresh();
            }
            return true;

        // Instant refresh when plugged in/plugged out
        case UiPageEventType::ChargeStateChanged:
            // Call to change status bar UI
            RefreshStatusBar();

            if(host_ != nullptr) {
                host_->RequestUrgentRefresh();
            }
            return true;
        
        default:
            return false;
    }
}
