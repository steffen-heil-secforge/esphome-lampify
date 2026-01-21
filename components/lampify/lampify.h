#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/preferences.h"
#include "esphome/core/log.h"
#include "esphome/components/esp32_ble/ble.h"
#include <esp_bt.h>
#include <esp_gap_ble_api.h>
#include <esp_bt_main.h>

namespace esphome {
namespace lampify {

class Lampify : public Component {
 public:
  void set_device_id(uint16_t device_id);
  uint16_t get_device_id() const { return device_id_; }
  const char* get_last_packet() const { return last_packet_hex_; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_BLUETOOTH; }

  void turn_on();
  void turn_off();
  void set_level(uint8_t cold, uint8_t warm);
  void pair();

  void add_on_device_id_change_callback(std::function<void()> callback) {
    this->device_id_change_callbacks_.add(std::move(callback));
  }

 protected:
  char last_packet_hex_[65] = {0};  // Store last sent packet as hex string
  uint16_t device_id_{0};
  ESPPreferenceObject pref_;
  CallbackManager<void()> device_id_change_callbacks_;

  static const uint8_t MIN_LEVEL = 3;
  static const uint8_t MAX_LEVEL = 255;

  static const uint16_t CRC_TABLE[256];
  static const uint8_t PACKET_BASE[32];

  uint8_t clamp_level(uint8_t val);
  uint16_t crc16(uint8_t *data, int offset);
  void bit_reverse(uint8_t *input, uint8_t *output, int len);
  void ble_whitening(uint8_t *input, uint8_t *output, int len);
  void ble_whitening_for_packet(uint8_t *input, uint8_t *output);
  void build_packet(uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t *packet);
  void send_packet(uint8_t *packet);
};

// Global instance for services
extern Lampify *global_lampify;

template<typename... Ts> class TurnOnAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  void play(Ts... x) override { this->parent_->turn_on(); }
};

template<typename... Ts> class TurnOffAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  void play(Ts... x) override { this->parent_->turn_off(); }
};

template<typename... Ts> class SetLevelAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  TEMPLATABLE_VALUE(uint8_t, cold)
  TEMPLATABLE_VALUE(uint8_t, warm)
  void play(Ts... x) override { this->parent_->set_level(this->cold_.value(x...), this->warm_.value(x...)); }
};

template<typename... Ts> class PairAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  void play(Ts... x) override { this->parent_->pair(); }
};

}  // namespace lampify
}  // namespace esphome
