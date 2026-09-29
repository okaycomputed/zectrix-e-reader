#include "pages/library.h"

#include "lcd_display.h"
#include "lvgl_theme.h"
#include "board.h"

#include <esp_log.h>

namespace {
constexpr lv_coord_t kPageWidth = 400;
constexpr lv_coord_t kPageHeight = 300;
constexpr lv_coord_t kTabBarHeight = 90;
constexpr char kTag[] = "Library";
} //namespace

Library::Library(LcdDisplay* host) : host_(host) {}

Library::~Library() {
    if (header != nullptr) {
        page_header_delete(header);
        header = nullptr;
    }
}

UiPageId Library::Id() const {
    return UiPageId::Library;
}

const char* Library::Name() const {
    return "Library";
}

// Retrieves book in accordance to the index
const Book& Library::GetBook(Tab tab) const {
    return books_[static_cast<int>(tab)];
}

const lv_font_t* Library::CurrentFont() const {
    if (host_ == nullptr) {
        return nullptr;
    }
    auto* theme = static_cast<LvglTheme*>(host_->GetTheme());
    if (theme == nullptr || theme->text_font() == nullptr) {
        return nullptr;
    }
    return theme->text_font()->font();
}

void Library::Build() {
    if (screen_ != nullptr) {
        return;
    }

    const lv_font_t* font = CurrentFont();

    // Screen LVGL object
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_size(screen_, kPageWidth, kPageHeight);
    lv_obj_set_style_bg_color(screen_, lv_color_white(), 0);
    lv_obj_set_style_border_width(screen_, 0, 0);
    lv_obj_set_style_pad_all(screen_, 0, 0);

    // Create the page header widget
    header = page_header_create(screen_, Name(), this, font);

    BuildBookTab();
    SelectFocus(0);

}

// Builds the vertical tab strip of each book inside the library.
// Placeholder books have been used as of now.
void Library::BuildBookTab() {
    const lv_font_t* font = CurrentFont();
    
    // Builds the parent book tab
    tab_bar_ = lv_obj_create(screen_);
    lv_obj_set_pos(tab_bar_, 0, 28);
    lv_obj_set_size(tab_bar_, kPageWidth, kPageHeight - 28);
    lv_obj_set_style_bg_color(tab_bar_, lv_color_white(), 0);
    lv_obj_set_style_pad_all(tab_bar_, 4, 0);

    // Make the book list scrollable
    lv_obj_add_flag(tab_bar_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(tab_bar_, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(tab_bar_, LV_SCROLL_SNAP_NONE);
    
    // Vertical flexing
    lv_obj_set_layout(tab_bar_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(tab_bar_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tab_bar_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    
    // Initializing each book tab
    for (int i = 0; i < kTabCount; i++) {
        // Creates a tab obj (child), inside tab_bar_ (parent)
        lv_obj_t* tab = lv_obj_create(tab_bar_);

        // Retrieve the current book
        const Book& book = GetBook(static_cast<Library::Tab>(i));

        if (font != nullptr) {
            lv_obj_set_style_text_font(tab, font, 0);
        }

        // General styling
        lv_obj_set_width(tab, lv_pct(100));
        lv_obj_set_height(tab, kTabBarHeight);
        lv_obj_set_style_text_align(tab, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_pad_all(tab, 8, 0);
        lv_obj_set_style_bg_opa(tab, LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_color(tab, lv_color_black(), 0); 

        // Vertical flexing
        lv_obj_set_layout(tab, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(tab, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

        // Displaying book title
        lv_obj_t* title = lv_label_create(tab);
        lv_label_set_text(title, book.title);

        // Displaying book author
        lv_obj_t* author = lv_label_create(tab);
        lv_label_set_text(author, book.author);

        // Displaying reading progress
        lv_obj_t* progress_row = lv_obj_create(tab);

        lv_obj_set_width(progress_row, lv_pct(100));
        lv_obj_set_height(progress_row, 20);

        lv_obj_set_style_pad_all(progress_row, 0, 0);
        lv_obj_set_style_border_width(progress_row, 0, 0);
        lv_obj_set_style_bg_opa(progress_row, LV_OPA_TRANSP, 0);

        // In-line horizontal flex for the progress bar and the percentage indicator 
        lv_obj_set_layout(progress_row, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(progress_row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(progress_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

        // Displaying progress bar
        lv_obj_t* progress_bar = lv_bar_create(progress_row);

        lv_obj_set_width(progress_bar, 260);
        lv_obj_set_height(progress_bar, 10);

        lv_bar_set_range(progress_bar, 0, 100);
        lv_bar_set_value(progress_bar, book.progress, LV_ANIM_OFF);
        lv_obj_set_style_border_width(progress_bar, 1, 0);
        lv_obj_set_style_border_color(progress_bar, lv_color_black(), 0);

        // Displaying progress percentage
        lv_obj_t* progress_label = lv_label_create(progress_row);
        lv_label_set_text_fmt(progress_label, "%d%%",book.progress);

        tab_labels_[i] = tab;
    }
}

// Moves focus through [tab 0, tab 1, ..., tab N-1, Back], wrapping in both
// directions, and updates the visual highlight to match.
void Library::SelectFocus(int index) {
    // Wraps 'index' so that it always stays between 0 and kFocusCount - 1, even when index is negative.
    focus_index_ = ((index % kFocusCount) + kFocusCount) % kFocusCount;
    ApplyFocusStyles();
}

void Library::ApplyFocusStyles() {
    for (int i = 0; i < kTabCount; i++) {
        lv_obj_t* tab = tab_labels_[i];

        if (tab == nullptr) {
            continue;
        }

        const bool selected = (i == focus_index_);
        
        // Outlines selected tab
        lv_obj_set_style_border_color(tab, selected ? lv_color_black() : lv_color_white(), 0);
        lv_obj_set_style_border_width(tab, 1, 0);

        // Scroll the selected tab into view
        if (selected) {
            lv_obj_scroll_to_view(tab, LV_ANIM_OFF);
        }
    }
    
    // If the back button's index, kTabCount, is in focus, send a signal to pageHeader to highlight it
    page_header_set_back_focused(header, focus_index_ == kBackFocusIndex);

}

lv_obj_t* Library::Screen() const {
    return screen_;
}

void Library::OnShow() {
    
}

void Library::OnHide() {
    
}  

bool Library::HandleEvent(const UiPageEvent& event) {
    switch (event.type) {
        case UiPageEventType::ButtonUp:
            SelectFocus(focus_index_ - 1);
            if (host_ != nullptr) {
                host_->RequestUrgentRefresh();
            }
            return true;
 
        case UiPageEventType::ButtonDown:
            SelectFocus(focus_index_ + 1);
            if (host_ != nullptr) {
                host_->RequestUrgentRefresh();
            }
            return true;
 
        case UiPageEventType::ButtonConfirm:
            ActivateFocused();
            return true;

        case UiPageEventType::ButtonConfirmLongPress:
            if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::ScreenSaver)) {
                host_->RequestUrgentFullRefresh();
            }
            return true;    
 
        default:
            return false;
    }
}

void Library::ActivateFocused() {
    if (focus_index_ == kBackFocusIndex) {
        GoBack();
        return;
    }
 
    // Hook: this is where per-tab activation logic goes (e.g. open the
    // selected book list). Currently a no-op beyond logging.
}
 
void Library::GoBack() {
    if (host_ != nullptr && host_->SwitchPageFromEvent(UiPageId::Home)) {
        host_->RequestUrgentFullRefresh();
    }
}