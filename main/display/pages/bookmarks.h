#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include "ui_page.h"
#include "page_header.h"

class LcdDisplay;

class Bookmarks : public IUiPage {
public:
    explicit Bookmarks(LcdDisplay* host);
    ~Bookmarks() override;

    UiPageId Id() const override;
    const char* Name() const override;
    void Build() override;
    lv_obj_t* Screen() const override;
    void OnShow() override;
    void OnHide() override;
    bool HandleEvent(const UiPageEvent& event) override;

private:
    const lv_font_t* CurrentFont() const;

    LcdDisplay* host_ = nullptr;
    lv_obj_t* screen_ = nullptr;
    page_header_t* header = nullptr;

    lv_obj_t* placeholder_label_ = nullptr;

};

#endif  // BOOKMARKS_H