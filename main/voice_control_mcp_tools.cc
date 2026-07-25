#include "voice_control_mcp_tools.h"

#include "application.h"
#include "board.h"
#include "display/smart_gadget_display.h"
#include "mcp_server.h"
#include "sensor_service.hpp"

#include <esp_err.h>

namespace {

cJSON* BuildEnvironmentSnapshot() {
    auto& sensor = SensorService::getInstance();
    esp_err_t init_result = ESP_OK;
    if (!sensor.isInitialized()) {
        init_result = sensor.init();
    }

    SensorData data = {};
    const bool valid = init_result == ESP_OK && sensor.getLatestData(data);
    cJSON* result = cJSON_CreateObject();
    cJSON_AddBoolToObject(result, "initialized", sensor.isInitialized());
    cJSON_AddBoolToObject(result, "valid", valid);
    cJSON_AddNumberToObject(result, "updated_at_us", static_cast<double>(data.updated_at_us));
    cJSON_AddNumberToObject(result, "valid_fields", data.valid_fields);
    if (init_result != ESP_OK) {
        cJSON_AddStringToObject(result, "error", esp_err_to_name(init_result));
    }
    if (valid) {
        cJSON_AddNumberToObject(result, "temperature_c", data.temperature);
        cJSON_AddNumberToObject(result, "humidity_percent", data.humidity);
        cJSON_AddNumberToObject(result, "pressure_pa", data.pressure);
        cJSON_AddNumberToObject(result, "iaq", data.iaq);
        cJSON_AddNumberToObject(result, "iaq_accuracy", data.iaq_accuracy);
        cJSON_AddNumberToObject(result, "co2_equivalent_ppm", data.co2_equivalent);
    }
    return result;
}

SmartGadgetDisplay* GetSmartGadgetDisplay() {
    return dynamic_cast<SmartGadgetDisplay*>(Board::GetInstance().GetDisplay());
}

SmartGadgetDisplay::ExternalPage ParsePage(const std::string& page) {
    if (page == "clock") return SmartGadgetDisplay::ExternalPage::Clock;
    if (page == "focus") return SmartGadgetDisplay::ExternalPage::Focus;
    if (page == "music") return SmartGadgetDisplay::ExternalPage::Music;
    if (page == "weather") return SmartGadgetDisplay::ExternalPage::Weather;
    if (page == "device") return SmartGadgetDisplay::ExternalPage::Device;
    throw std::invalid_argument("Unsupported page: " + page);
}

ReturnValue RunDeviceAction(const PropertyList& properties) {
    const std::string action = properties["action"].value<std::string>();
    auto* display = GetSmartGadgetDisplay();

    if (action == "music.play") return Application::GetInstance().SetMusicPlayback(true);
    if (action == "music.pause") return Application::GetInstance().SetMusicPlayback(false);

    if (display == nullptr) {
        throw std::runtime_error("Smart gadget display is unavailable");
    }
    if (action == "focus.start") {
        return display->StartFocusForMinutes(properties["minutes"].value<int>());
    }
    if (action == "focus.pause") return display->PauseFocus();
    if (action == "focus.resume") return display->ResumeFocus();
    if (action == "focus.stop") return display->StopFocus();
    if (action == "page.open") {
        return display->OpenPage(ParsePage(properties["page"].value<std::string>()));
    }
    throw std::invalid_argument("Unsupported device action: " + action);
}

}  // namespace

void RegisterVoiceControlMcpTools(McpServer& server) {
    server.AddTool("self.environment.get_latest",
        "Read the latest BME690/BSEC environment snapshot. Always inspect `valid` before using values.",
        PropertyList(),
        [](const PropertyList&) -> ReturnValue {
            return BuildEnvironmentSnapshot();
        });

    server.AddTool("self.device.control",
        "Control the device. Actions: focus.start, focus.pause, focus.resume, focus.stop, music.play, music.pause, page.open.",
        PropertyList({
            Property("action", kPropertyTypeString),
            Property("minutes", kPropertyTypeInteger, 25, 1, 60),
            Property("page", kPropertyTypeString, std::string("clock")),
        }),
        [](const PropertyList& properties) -> ReturnValue {
            // McpServer::DoToolCall dispatches this callback through Application::Schedule.
            return RunDeviceAction(properties);
        });
}
