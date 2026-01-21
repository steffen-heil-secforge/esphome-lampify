#pragma once

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "lampify.h"

namespace esphome {
namespace lampify {

class LampifyPacketSensor : public text_sensor::TextSensor, public PollingComponent {
 public:
  void set_parent(Lampify *parent) { this->parent_ = parent; }

  void update() override {
    const char *packet = this->parent_->get_last_packet();
    if (packet != nullptr && packet[0] != '\0') {
      this->publish_state(packet);
    }
  }

  void dump_config() override {
    ESP_LOGCONFIG("lampify.text_sensor", "Lampify Packet Sensor");
  }

 protected:
  Lampify *parent_{nullptr};
};

}  // namespace lampify
}  // namespace esphome
