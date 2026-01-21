#include "light_output.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lampify {

static const char *const TAG = "lampify.light";

void LampifyLight::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Lampify Light");
}

void LampifyLight::dump_config() {
  ESP_LOGCONFIG(TAG, "Lampify Light:");
  ESP_LOGCONFIG(TAG, "  Lamp index: %d", this->lamp_index_);
}

light::LightTraits LampifyLight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::COLD_WARM_WHITE});
  traits.set_min_mireds(153);   // ~6500K cold
  traits.set_max_mireds(370);   // ~2700K warm
  return traits;
}

void LampifyLight::write_state(light::LightState *state) {
  if (this->parent_ == nullptr) {
    ESP_LOGE(TAG, "Parent is null, cannot control lamp!");
    return;
  }

  // Get raw state values
  bool is_on = state->current_values.is_on();
  float brightness = state->current_values.get_brightness();
  float color_temp_mireds = state->current_values.get_color_temperature();

  if (is_on && brightness > 0.0f) {
    // If lamp was off, send turn_on command first
    if (!last_state_) {
      this->parent_->turn_on(this->lamp_index_);
      last_state_ = true;
    }

    // Convert brightness (0-1) to total level
    uint8_t total = static_cast<uint8_t>(brightness * 255);
    if (total < 6) total = 6;  // Minimum so cold/warm can both be >= 3

    // Normalize color temp from mireds to 0-1 (cold to warm)
    // min_mireds = 153 (6500K, cold), max_mireds = 370 (2700K, warm)
    const float min_mireds = 153.0f;
    const float max_mireds = 370.0f;
    float color_temp_normalized = 0.5f;  // Default to middle

    if (color_temp_mireds >= min_mireds && color_temp_mireds <= max_mireds) {
      color_temp_normalized = (color_temp_mireds - min_mireds) / (max_mireds - min_mireds);
    } else if (color_temp_mireds > max_mireds) {
      color_temp_normalized = 1.0f;  // Warmest
    } else if (color_temp_mireds < min_mireds && color_temp_mireds > 0) {
      color_temp_normalized = 0.0f;  // Coldest
    }

    // Calculate cold and warm based on color temperature
    // When color_temp_normalized = 0: all cold, no warm
    // When color_temp_normalized = 1: no cold, all warm
    // When color_temp_normalized = 0.5: equal cold and warm
    uint8_t warm = static_cast<uint8_t>(color_temp_normalized * total);
    uint8_t cold = total - warm;

    // Ensure minimum levels
    if (cold < 3) cold = 3;
    if (warm < 3) warm = 3;

    // Only send set_level if brightness/color changed
    if (cold != last_cold_ || warm != last_warm_) {
      this->parent_->set_level(this->lamp_index_, cold, warm);
      last_cold_ = cold;
      last_warm_ = warm;
    }
  } else {
    if (last_state_) {
      this->parent_->turn_off(this->lamp_index_);
      last_state_ = false;
    }
  }
}

}  // namespace lampify
}  // namespace esphome
