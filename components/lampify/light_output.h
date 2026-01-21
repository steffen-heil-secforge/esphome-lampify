#pragma once

#include "lampify.h"
#include "esphome/components/light/light_output.h"

namespace esphome {
namespace lampify {

class LampifyLight : public light::LightOutput, public Component {
 public:
  void set_parent(Lampify *parent) { this->parent_ = parent; }
  void set_lamp_index(uint8_t index) { this->lamp_index_ = index; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_BLUETOOTH; }

  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;

 protected:
  Lampify *parent_{nullptr};
  uint8_t lamp_index_{0};
  bool last_state_{false};
  uint8_t last_cold_{0};
  uint8_t last_warm_{0};
};

}  // namespace lampify
}  // namespace esphome
