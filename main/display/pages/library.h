#ifndef LIBRARY_H
#define LIBRARY_H

#include "ui_page.h"
#include "page_header.h"

class LcdDisplay;

// Stores the details of each book inside the library
struct Book {
    const char *title;
    const char *author;
    uint8_t progress;
};

class Library : public IUiPage {
public:
    explicit Library(LcdDisplay* host);
    ~Library() override;

    UiPageId Id() const override;
    const char* Name() const override;
    void Build() override;
    lv_obj_t* Screen() const override;
    void OnShow() override;
    void OnHide() override;
    bool HandleEvent(const UiPageEvent& event) override;

private:
    enum class Tab : int {
        Book1 = 0,
        Book2,
        Book3,
        Book4,
        kCount,
    };

    Book books_[4] = {
        {"Giovanni's Room", "James Baldwin", 42},
        {"1984", "George Orwell", 75},
        {"Cocaine Nights", "J.G. Ballard", 20},
        {"The Handmaid's Tale", "Margaret Atwood", 1},
    };

    // Total number of tabs
    static constexpr int kTabCount = static_cast<int>(Tab::kCount);

    // Focus indices 0..kTabCount-1 are the tabs; kTabCount is "Back".
    //(Last tab is the "back" button)
    static constexpr int kFocusCount = kTabCount + 1;
    static constexpr int kBackFocusIndex = kTabCount;

    void BuildBookTab();
    void SelectFocus(int index);
    void ApplyFocusStyles();
    void ActivateFocused();
    void GoBack();
    const Book& GetBook(Tab tab) const;
    const lv_font_t* CurrentFont() const;

    LcdDisplay* host_ = nullptr;
    lv_obj_t* screen_ = nullptr;
    page_header_t* header = nullptr;

    lv_obj_t* tab_bar_ = nullptr;
    lv_obj_t* tab_labels_[kTabCount] = {nullptr, nullptr, nullptr, nullptr};
    lv_obj_t* progress_bars_[kTabCount] = {nullptr};

    int focus_index_ = 0;

};

#endif  // LIBRARY_H