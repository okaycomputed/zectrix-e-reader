#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "lvgl_display.h"
#include "ui_page_registry.h"

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

#include <memory>

class LcdDisplay : public LvglDisplay {
protected:
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;

    UiPageRegistry page_registry_;
    bool ui_setup_done_ = false;

    void ShowScreen(lv_obj_t* scr);
    bool RegisterPageLocked(std::unique_ptr<IUiPage> page);
    bool SwitchPageLocked(UiPageId id);

    // Builds  initial pages and registers them
    void SetupUI();

    bool Lock(int timeout_ms = 0) override;
    void Unlock() override;

    LcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel, int width, int height);

public:
    ~LcdDisplay() override;

    void SetEmotion(const char* emotion) override;
    void SetChatMessage(const char* role, const char* content) override;
    void SetPreviewImage(std::unique_ptr<LvglImage> image);
    void SetTheme(Theme* theme) override;

    bool RegisterPage(std::unique_ptr<IUiPage> page);
    bool SwitchPage(UiPageId id);
    bool SwitchPageFromEvent(UiPageId id);
    UiPageId GetActivePageId() const;
    void DispatchPageEvent(const UiPageEvent& e, bool only_active = true);

    // Called once at boot to put the device into its main UI.
    void EnterMainFlow();
};

#endif  // LCD_DISPLAY_H
