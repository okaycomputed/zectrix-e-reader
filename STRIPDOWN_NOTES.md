# What was removed / what's left

Starting point was the developer's official open-source archive
(`zectrix.tar.gz`). This is that same project with the application-level
software removed, keeping only hardware bring-up.

## Removed entirely
- `main/audio/` — the AI-assistant audio pipeline (codec service, wake word,
  audio processors). `Board::GetAudioCodec()` now defaults to `nullptr`
  instead of being mandatory.
- `main/protocols/` — AI chat protocol headers (unused once audio's gone).
- `main/boards/zectrix-s3-epaper-4.2/FT/` — the factory-test service/flow.
- `main/display/pages/factory_test_page_adapter.cc/h` — the only UI screen
  that shipped.

## Renamed for clarity (same hardware behavior, less confusing names)
- `Board::EnterFactoryTestFlow()` -> `Board::EnterMainFlow()`
- `LcdDisplay::ShowFactoryTestPage()` / `IsFactoryTestPageActive()` -> gone;
  replaced by generic `RegisterPage()` / `SwitchPage()` you already have,
  plus `EnterMainFlow()` which builds + shows the first page.
- `UiPageId::FactoryTest` -> `UiPageId::Blank`
- A few FT-suffixed helper function names on `CustomBoard` (battery/charge
  reads) — same logic, just dropped the "ForFactoryTest" suffix.

## Kept as-is (this is your hardware layer — don't need to touch it)
- Power sequencing, I2C bus, RTC (PCF8563), NFC (GT23SC6699), charge status
- `CustomLcdDisplay` — the raw e-paper SPI driver, partial/full refresh
  logic, and the low-level `DrawTexts()`/`WriteRaw1bpp()` primitives
- Buttons — now wired to plain `ESP_LOGI` placeholders instead of factory
  test actions; rewire these to your own logic in
  `boards/zectrix-s3-epaper-4.2/zectrix-s3-epaper-4.2.cc` -> `InitializeButtons()`
- The page-plugin architecture (`IUiPage` / `UiPageRegistry`)

## Your starting point
`main/display/pages/blank_page.cc` — one `IUiPage` with an empty white LVGL
screen and a single placeholder label. Registered and shown automatically
at boot via `LcdDisplay::SetupUI()` -> `Application::Initialize()`.

## Not verified
I edited this by hand and checked it for dangling references to deleted
files/types (no leftover includes, no orphaned enum cases, balanced
braces) — but I do not have ESP-IDF or network access in my environment, so
**this has not actually been compiled**. Run `./build.sh` and send me the
first error if anything doesn't build; this kind of removal is the most
likely place for a one-line mistake to slip through a manual edit.
