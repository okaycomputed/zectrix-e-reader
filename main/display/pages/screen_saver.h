#ifndef SCREEN_SAVER_H
#define SCREEN_SAVER_H

#include "ui_page.h"

class LcdDisplay;

// Screensaver: Displays when no buttons have been pressed in 15 minutes
//            -> When long pressed, the select button can immediately trigger the screensaver

class ScreenSaver : public IUiPage {
public:
    explicit ScreenSaver(LcdDisplay* host);
    ~ScreenSaver() override;

    UiPageId Id() const override;
    const char* Name() const override;
    void Build() override;
    lv_obj_t* Screen() const override;
    void OnShow() override;
    void OnHide() override;
    bool HandleEvent(const UiPageEvent& event) override;

private:
    const lv_font_t* CurrentFont() const;
    void LoadScreensaverImage();

    LcdDisplay* host_ = nullptr;
    lv_obj_t* screen_ = nullptr;

    lv_obj_t* screen_saver_text_ = nullptr;
    lv_obj_t* nudaeng_img_ = nullptr;

};

#endif  // SCREEN_SAVER_H