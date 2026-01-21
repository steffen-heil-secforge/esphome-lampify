#pragma once

#include "lampify.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace lampify {

class LampifyDeviceIdNumber : public number::Number, public Component {
 public:
  void set_parent(Lampify *parent) { this->parent_ = parent; }
  void set_lamp_index(uint8_t index) { this->lamp_index_ = index; }

  float get_setup_priority() const override { return setup_priority::LATE; }

  void setup() override {
    float value = this->parent_->get_device_id(this->lamp_index_);
    this->publish_state(value);

    this->parent_->add_on_device_id_change_callback(this->lamp_index_, [this]() {
      this->publish_state(this->parent_->get_device_id(this->lamp_index_));
    });
  }

  void dump_config() override {
    ESP_LOGCONFIG("lampify.number", "Lampify Device ID Number (lamp %d)", this->lamp_index_);
  }

 protected:
  void control(float value) override {
    uint16_t device_id = static_cast<uint16_t>(value);
    this->parent_->set_device_id(this->lamp_index_, device_id);
    this->publish_state(value);
  }

  Lampify *parent_{nullptr};
  uint8_t lamp_index_{0};
};

}  // namespace lampify
}  // namespace esphome
