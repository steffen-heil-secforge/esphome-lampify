#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/preferences.h"
#include "esphome/core/log.h"
#include "esphome/components/esp32_ble/ble.h"
#include <esp_bt.h>
#include <esp_gap_ble_api.h>
#include <esp_bt_main.h>
#include <vector>

namespace esphome {
namespace lampify {

static const uint8_t MAX_LAMPS = 16;

class Lampify : public Component {
 public:
  void set_num_lamps(uint8_t num) { num_lamps_ = num; }
  uint8_t get_num_lamps() const { return num_lamps_; }

  void set_device_id(uint8_t lamp_index, uint16_t device_id);
  uint16_t get_device_id(uint8_t lamp_index) const;
  const char* get_last_packet() const { return last_packet_hex_; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_BLUETOOTH; }

  // Commands with lamp index
  void turn_on(uint8_t lamp_index);
  void turn_off(uint8_t lamp_index);
  void set_level(uint8_t lamp_index, uint8_t cold, uint8_t warm);
  void pair(uint8_t lamp_index);

  // Legacy single-lamp commands (use lamp 0)
  void turn_on() { turn_on(0); }
  void turn_off() { turn_off(0); }
  void set_level(uint8_t cold, uint8_t warm) { set_level(0, cold, warm); }
  void pair() { pair(0); }

  void add_on_device_id_change_callback(uint8_t lamp_index, std::function<void()> callback);

 protected:
  char last_packet_hex_[65] = {0};
  uint8_t num_lamps_{8};
  uint16_t device_ids_[MAX_LAMPS] = {0};
  ESPPreferenceObject prefs_[MAX_LAMPS];
  CallbackManager<void()> device_id_change_callbacks_[MAX_LAMPS];

  static const uint8_t MIN_LEVEL = 3;
  static const uint8_t MAX_LEVEL = 255;

  static const uint16_t CRC_TABLE[256];
  static const uint8_t PACKET_BASE[32];

  uint8_t clamp_level(uint8_t val);
  uint16_t crc16(uint8_t *data, int offset);
  void bit_reverse(uint8_t *input, uint8_t *output, int len);
  void ble_whitening(uint8_t *input, uint8_t *output, int len);
  void ble_whitening_for_packet(uint8_t *input, uint8_t *output);
  void build_packet(uint16_t device_id, uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t *packet);
  void send_packet(uint8_t *packet);
};

// Global instance for services
extern Lampify *global_lampify;

template<typename... Ts> class TurnOnAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  TEMPLATABLE_VALUE(uint8_t, lamp_index)
  void play(Ts... x) override { this->parent_->turn_on(this->lamp_index_.value(x...)); }
};

template<typename... Ts> class TurnOffAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  TEMPLATABLE_VALUE(uint8_t, lamp_index)
  void play(Ts... x) override { this->parent_->turn_off(this->lamp_index_.value(x...)); }
};

template<typename... Ts> class SetLevelAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  TEMPLATABLE_VALUE(uint8_t, lamp_index)
  TEMPLATABLE_VALUE(uint8_t, cold)
  TEMPLATABLE_VALUE(uint8_t, warm)
  void play(Ts... x) override {
    this->parent_->set_level(this->lamp_index_.value(x...), this->cold_.value(x...), this->warm_.value(x...));
  }
};

template<typename... Ts> class PairAction : public Action<Ts...>, public Parented<Lampify> {
 public:
  TEMPLATABLE_VALUE(uint8_t, lamp_index)
  void play(Ts... x) override { this->parent_->pair(this->lamp_index_.value(x...)); }
};

}  // namespace lampify
}  // namespace esphome
