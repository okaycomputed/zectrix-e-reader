#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include "ui_page.h"

class LcdDisplay;

// Home page: status bar (date / time / battery) + a 4-tab strip (Library,
// Bookmarks, Upload, Settings) + a content area below.
//
// Navigation (no touchscreen on this panel, so it's button-driven):
//   Up / Down     -> move tab selection left / right
//   Confirm       -> "activate" the selected tab (currently just logs;
//                    this is the hook to wire real per-tab behavior into)
class HomePage : public IUiPage {
public:
    explicit HomePage(LcdDisplay* host);
    ~HomePage() override;

    UiPageId Id() const override;
    const char* Name() const override;
    void Build() override;
    lv_obj_t* Screen() const override;
    void OnShow() override;
    void OnHide() override;
    bool HandleEvent(const UiPageEvent& event) override;

private:
    enum class Tab : int {
        Library = 0,
        Bookmarks,
        Upload,
        Settings,
        kCount,
    };

    void BuildStatusBar();
    void BuildTabBar();
    void SelectTab(int index);
    void RefreshStatusBar();
    const lv_font_t* CurrentFont() const;
    static const char* TabName(Tab tab);
    static void StatusTimerCallback(lv_timer_t* timer);

    LcdDisplay* host_ = nullptr;
    lv_obj_t* screen_ = nullptr;

    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* date_time_label_ = nullptr;
    lv_obj_t* battery_label_ = nullptr;

    lv_obj_t* tab_bar_ = nullptr;
    lv_obj_t* tab_labels_[static_cast<int>(Tab::kCount)] = {nullptr, nullptr, nullptr, nullptr};

    int selected_tab_ = 0;
    lv_timer_t* status_timer_ = nullptr;
};

#endif  // HOME_PAGE_H
