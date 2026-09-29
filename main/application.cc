#include "application.h"

#include "board.h"
#include "display.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace {

constexpr char kTag[] = "Application";

// CPU frequency range for dynamic frequency scaling. The system runs at
// kMaxCpuFreqMhz under normal use and is allowed to drop to kMinCpuFreqMhz
// (and to automatic light sleep) once SleepManager reports the device is
// idle, e.g. while the screensaver is showing.
constexpr int kMaxCpuFreqMhz = 240;
constexpr int kMinCpuFreqMhz = 40;
 
void ConfigurePowerManagement() {
    esp_pm_config_t pm_config = {};
    pm_config.max_freq_mhz = kMaxCpuFreqMhz;
    pm_config.min_freq_mhz = kMinCpuFreqMhz;
    pm_config.light_sleep_enable = true;
 
    esp_err_t err = esp_pm_configure(&pm_config);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "esp_pm_configure failed: %s", esp_err_to_name(err));
    }
}

}  // namespace

Application::Application() = default;

Application::~Application() = default;

void Application::Initialize() {
    ConfigurePowerManagement();

    auto& board = Board::GetInstance();
    SetDeviceState(kDeviceStateStarting);

    Display* display = board.GetDisplay();
    if (display != nullptr) {
        display->UpdateStatusBar(true);
    }

    SetDeviceState(kDeviceStateIdle);

    // Hand off to the board's own UI flow. This
    // is the hook to override if you want a different boot sequence.
    board.EnterMainFlow();
}

void Application::Run() {
    while (true) {
        vTaskDelay(portMAX_DELAY);
    }
}

bool Application::SetDeviceState(DeviceState state) {
    const DeviceState old_state = state_.exchange(state, std::memory_order_acq_rel);
    ESP_LOGI(kTag, "State %d -> %d", old_state, state);
    return true;
}

void Application::Schedule(std::function<void()>&& callback) {
    if (callback) {
        callback();
    }
}

bool Application::CanEnterSleepMode() const {
    switch (GetDeviceState()) {
        case kDeviceStateIdle:
        case kDeviceStateListening:
            ESP_LOGI(kTag, "Sleeping now");
            return true;
        default:
            // Starting up, connecting, upgrading, activating, in a fatal
            // error, etc. — don't let the system drop CPU frequency or
            // light-sleep mid-operation.
            ESP_LOGI(kTag, "Unable to sleep");
            return false;
    }
}
