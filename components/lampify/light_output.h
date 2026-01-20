#pragma once

#include "lampify.h"
#include "esphome/components/light/light_output.h"

namespace esphome {
namespace lampify {

class LampifyLight : public light::LightOutput, public Component {
 public:
  void set_device_id(uint16_t device_id) { device_id_ = device_id; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_BLUETOOTH; }

  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;

 protected:
  uint16_t device_id_{0};
  Lampify controller_;
  bool last_state_{false};
  uint8_t last_cold_{0};
  uint8_t last_warm_{0};
};

}  // namespace lampify
}  // namespace esphome
