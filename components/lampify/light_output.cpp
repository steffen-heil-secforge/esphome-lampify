#include "light_output.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lampify {

static const char *const TAG = "lampify.light";

void LampifyLight::setup() {
  controller_.set_device_id(device_id_);
  controller_.setup();
  ESP_LOGCONFIG(TAG, "Setting up Lampify Light with device ID: 0x%04X", device_id_);
}

void LampifyLight::dump_config() {
  ESP_LOGCONFIG(TAG, "Lampify Light:");
  ESP_LOGCONFIG(TAG, "  Device ID: 0x%04X", device_id_);
}

light::LightTraits LampifyLight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::COLD_WARM_WHITE});
  traits.set_min_mireds(153);   // ~6500K cold
  traits.set_max_mireds(370);   // ~2700K warm
  return traits;
}

void LampifyLight::write_state(light::LightState *state) {
  float brightness;
  float color_temp;

  state->current_values_as_ct(&color_temp, &brightness);

  bool is_on = brightness > 0.0f;

  if (is_on) {
    // Convert brightness (0-1) and color_temp (0-1, cold to warm) to cold/warm levels
    // color_temp: 0 = cold (6500K), 1 = warm (2700K)
    uint8_t total = static_cast<uint8_t>(brightness * 255);
    if (total < 3) total = 3;

    // Calculate cold and warm based on color temperature
    // When color_temp = 0: all cold, no warm
    // When color_temp = 1: no cold, all warm
    // When color_temp = 0.5: equal cold and warm
    uint8_t warm = static_cast<uint8_t>(color_temp * total);
    uint8_t cold = total - warm;

    // Ensure minimum levels
    if (cold < 3) cold = 3;
    if (warm < 3) warm = 3;

    // Only send if changed
    if (!last_state_ || cold != last_cold_ || warm != last_warm_) {
      controller_.set_level(cold, warm);
      last_cold_ = cold;
      last_warm_ = warm;
    }

    if (!last_state_) {
      last_state_ = true;
    }
  } else {
    if (last_state_) {
      controller_.turn_off();
      last_state_ = false;
    }
  }
}

}  // namespace lampify
}  // namespace esphome
