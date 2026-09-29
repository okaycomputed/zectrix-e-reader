set(SOURCES
    "application.cc"
    "main.cc"
    "settings.cc"
    "system_info.cc"
    "display/display.cc"
    "display/lcd_display.cc"
    "display/ui_page_registry.cc"
    "display/pages/home_page.cc"
    "display/pages/screen_saver.cc"
    "display/pages/library.cc"
    "display/pages/bookmarks.cc"
    "display/pages/upload.cc"
    "display/pages/user_settings.cc"
    "display/lvgl_display/lvgl_display.cc"
    "display/lvgl_display/lvgl_theme.cc"
    "display/lvgl_display/lvgl_font.cc"
    "common/sleep_manager.cc"
    "boards/common/board.cc"
    "boards/common/button.cc"
    "boards/common/i2c_bus_lock.cc"
    "boards/common/i2c_device.cc"
    "boards/zectrix/zectrix_nfc.cc"
    "boards/zectrix-s3-epaper-4.2/board_power_bsp.cc"
    "boards/zectrix-s3-epaper-4.2/charge_status.cc"
    "boards/zectrix-s3-epaper-4.2/custom_lcd_display.cc"
    "boards/zectrix-s3-epaper-4.2/i2c_power_hook.cc"
    "boards/zectrix-s3-epaper-4.2/rtc_pcf8563.cc"
    "boards/zectrix-s3-epaper-4.2/zectrix-s3-epaper-4.2.cc"
    "display/assets/ReadingNudaeng.c"
    "display/widgets/page_header.cc"
)

set(INCLUDE_DIRS
    "."
    "display"
    "display/lvgl_display"
    "display/assets"
    "display/widgets"
    "boards/common"
    "network"
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDE_DIRS}
)

target_compile_definitions(
    ${COMPONENT_LIB}
    PRIVATE
    BOARD_TYPE=\"zectrix-s3-epaper-4.2\"
    BOARD_NAME=\"zectrix-s3-epaper-4.2\"
    BUILTIN_TEXT_FONT=SourceHanSansSC_Regular_slim
    BUILTIN_ICON_FONT=font_zectrix_16_1
)
