#pragma once

#include "lampify.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace lampify {

class LampifyPairButton : public button::Button, public Component {
 public:
  void set_parent(Lampify *parent) { this->parent_ = parent; }
  void set_lamp_index(uint8_t index) { this->lamp_index_ = index; }

  void dump_config() override {
    ESP_LOGCONFIG("lampify.button", "Lampify Pair Button (lamp %d)", this->lamp_index_);
  }

 protected:
  void press_action() override {
    this->parent_->pair(this->lamp_index_);
  }

  Lampify *parent_{nullptr};
  uint8_t lamp_index_{0};
};

}  // namespace lampify
}  // namespace esphome
