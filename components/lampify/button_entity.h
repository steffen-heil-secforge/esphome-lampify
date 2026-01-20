#pragma once

#include "lampify.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace lampify {

class LampifyPairButton : public button::Button, public Component {
 public:
  void set_parent(Lampify *parent) { this->parent_ = parent; }

  void dump_config() override {
    ESP_LOGCONFIG("lampify.button", "Lampify Pair Button");
  }

 protected:
  void press_action() override {
    this->parent_->pair();
  }

  Lampify *parent_{nullptr};
};

}  // namespace lampify
}  // namespace esphome
