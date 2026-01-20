#pragma once

#include "lampify.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace lampify {

class LampifyDeviceIdNumber : public number::Number, public Component {
 public:
  void set_parent(Lampify *parent) { this->parent_ = parent; }

  void setup() override {
    float value = this->parent_->get_device_id();
    this->publish_state(value);

    this->parent_->add_on_device_id_change_callback([this]() {
      this->publish_state(this->parent_->get_device_id());
    });
  }

  void dump_config() override {
    ESP_LOGCONFIG("lampify.number", "Lampify Device ID Number");
  }

 protected:
  void control(float value) override {
    uint16_t device_id = static_cast<uint16_t>(value);
    this->parent_->set_device_id(device_id);
    this->publish_state(value);
  }

  Lampify *parent_{nullptr};
};

}  // namespace lampify
}  // namespace esphome
