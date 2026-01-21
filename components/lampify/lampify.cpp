#include "lampify.h"
#include "esphome/core/log.h"
#include <cstring>
#include <esp_random.h>

namespace esphome {
namespace lampify {

static const char *const TAG = "lampify";

Lampify *global_lampify = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

const uint8_t Lampify::PACKET_BASE[32] = {
    0x1F, 0x02, 0x01, 0x01, 0x1B, 0x03, 0x71, 0x0F,
    0x55, 0xAA, 0x98, 0x43, 0xAF, 0x0B, 0x46, 0x46,
    0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint16_t Lampify::CRC_TABLE[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

void Lampify::setup() {
  global_lampify = this;

  // Load device_id from preferences
  this->pref_ = global_preferences->make_preference<uint16_t>(fnv1_hash("lampify_device_id"));
  uint16_t stored_id;
  if (this->pref_.load(&stored_id)) {
    this->device_id_ = stored_id;
    ESP_LOGCONFIG(TAG, "Loaded device ID from flash: 0x%04X", device_id_);
  } else {
    ESP_LOGCONFIG(TAG, "No stored device ID, using: 0x%04X", device_id_);
  }
}

void Lampify::dump_config() {
  ESP_LOGCONFIG(TAG, "Lampify:");
  ESP_LOGCONFIG(TAG, "  Device ID: 0x%04X", device_id_);
}

void Lampify::set_device_id(uint16_t device_id) {
  if (this->device_id_ != device_id) {
    this->device_id_ = device_id;
    this->pref_.save(&device_id);
    ESP_LOGI(TAG, "Device ID set to 0x%04X", device_id_);
    this->device_id_change_callbacks_.call();
  }
}

uint8_t Lampify::clamp_level(uint8_t val) {
  if (val < MIN_LEVEL) return MIN_LEVEL;
  if (val > MAX_LEVEL) return MAX_LEVEL;
  return val;
}

uint16_t Lampify::crc16(uint8_t *data, int offset) {
  uint16_t crc = 0xFFFF;
  for (int i = 0; i < 12; i++) {
    crc = CRC_TABLE[((crc >> 8) ^ data[offset + i]) & 0xFF] ^ (crc << 8);
  }
  return crc;
}

void Lampify::bit_reverse(uint8_t *input, uint8_t *output, int len) {
  for (int i = 0; i < len; i++) {
    uint8_t rev = 0;
    for (int j = 0; j < 8; j++) {
      rev |= ((input[i] >> (7 - j)) & 1) << j;
    }
    output[i] = rev;
  }
}

void Lampify::ble_whitening(uint8_t *input, uint8_t *output, int len) {
  int state = 83;
  for (int i = 0; i < len; i++) {
    int current_state = state;
    uint8_t byte = 0;
    for (int bit = 0; bit < 8; bit++) {
      int s = current_state & 0xFF;
      byte |= ((((s & 64) >> 6) << bit) ^ (input[i] & 0xFF)) & (1 << bit);
      int shifted = s << 1;
      int feedback = (shifted >> 7) & 1;
      int next = ((shifted & ~1) | feedback);
      current_state = ((next ^ (feedback << 4)) & 0x10) | (next & ~0x10);
    }
    output[i] = byte;
    state = current_state;
  }
}

void Lampify::ble_whitening_for_packet(uint8_t *input, uint8_t *output) {
  uint8_t temp_in[38] = {0};
  uint8_t temp_out[38] = {0};

  for (int i = 0; i < 25; i++) {
    temp_in[i + 13] = input[i];
  }

  ble_whitening(temp_in, temp_out, 38);

  for (int i = 0; i < 25; i++) {
    output[i] = temp_out[i + 13];
  }
}

void Lampify::build_packet(uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t *packet) {
  uint8_t msg_base[25];
  uint8_t msg_rev[25];
  uint8_t msg_wht[25];

  // Copy base message
  for (int i = 0; i < 25; i++) {
    msg_base[i] = PACKET_BASE[i + 6];
  }

  // Set command and arguments
  msg_base[11] = command;
  msg_base[12] = (device_id_ >> 8) & 0xFF;  // Master control high byte
  msg_base[13] = device_id_ & 0xFF;          // Master control low byte
  msg_base[14] = arg1;
  msg_base[15] = arg2;
  msg_base[17] = 0x42;  // Fixed value for testing - should match CLI with same device ID
  // msg_base[17] = esp_random() & 0xFF;        // Random byte (disabled for testing)

  // Calculate CRC
  uint16_t crc = crc16(msg_base, 11);
  msg_base[23] = (crc >> 8) & 0xFF;
  msg_base[24] = crc & 0xFF;

  // Apply bit reversal and whitening
  bit_reverse(msg_base, msg_rev, 25);
  ble_whitening_for_packet(msg_rev, msg_wht);

  // Build final packet
  for (int i = 0; i < 6; i++) {
    packet[i] = PACKET_BASE[i];
  }
  for (int i = 0; i < 25; i++) {
    packet[i + 6] = msg_wht[i];
  }
  packet[31] = PACKET_BASE[31];
}

void Lampify::send_packet(uint8_t *packet) {
  if (device_id_ == 0) {
    ESP_LOGW(TAG, "Device ID not set, cannot send packet");
    return;
  }

  // Log packet at INFO level for debugging and store for diagnostic sensor
  for (int i = 0; i < 32; i++) {
    sprintf(last_packet_hex_ + i * 2, "%02X", packet[i]);
  }
  last_packet_hex_[64] = '\0';
  ESP_LOGI(TAG, "Sending packet: %s", last_packet_hex_);

  // Use raw BLE advertising via ESP-IDF VHCI/controller layer
  // This bypasses the GAP layer which may be causing issues

  // First, stop any existing advertising
  esp_ble_gap_stop_advertising();
  delay(50);

  // HCI LE Set Advertising Parameters (OGF=0x08, OCF=0x0006)
  // Parameters: min_interval(2), max_interval(2), type(1), own_addr_type(1),
  //             peer_addr_type(1), peer_addr(6), channel_map(1), filter_policy(1)
  uint8_t adv_params_cmd[15] = {0};
  adv_params_cmd[0] = 0x20;  // min_interval low byte
  adv_params_cmd[1] = 0x00;  // min_interval high byte
  adv_params_cmd[2] = 0x20;  // max_interval low byte
  adv_params_cmd[3] = 0x00;  // max_interval high byte
  adv_params_cmd[4] = 0x00;  // adv_type = ADV_IND (connectable undirected)
  adv_params_cmd[5] = 0x00;  // own_addr_type = public
  adv_params_cmd[6] = 0x00;  // peer_addr_type = public
  // adv_params_cmd[7-12] = peer_addr (all zeros)
  adv_params_cmd[13] = 0x07; // channel_map = all channels
  adv_params_cmd[14] = 0x00; // filter_policy = allow all

  // Use esp_ble_gap functions but set params first
  esp_ble_adv_params_t adv_params = {};
  adv_params.adv_int_min = 0x20;
  adv_params.adv_int_max = 0x20;
  adv_params.adv_type = ADV_TYPE_IND;
  adv_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  adv_params.channel_map = ADV_CHNL_ALL;
  adv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

  // Set advertising data - send full 31 bytes (skip length byte)
  esp_err_t ret = esp_ble_gap_config_adv_data_raw(packet + 1, 31);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set advertising data: %s", esp_err_to_name(ret));
    return;
  }

  // Wait longer for async operation
  delay(200);

  // Start advertising
  ret = esp_ble_gap_start_advertising(&adv_params);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start advertising: %s", esp_err_to_name(ret));
    return;
  }

  ESP_LOGI(TAG, "Advertising started, sending for 500ms");

  // Advertise for 500ms (longer than CLI to ensure delivery)
  delay(500);

  // Stop advertising
  ret = esp_ble_gap_stop_advertising();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to stop advertising: %s", esp_err_to_name(ret));
  }

  ESP_LOGI(TAG, "Packet sent successfully");
}

void Lampify::turn_on() {
  ESP_LOGI(TAG, "Turning on lamp 0x%04X", device_id_);
  uint8_t packet[32];
  build_packet(0x10, 0x00, 0x00, packet);
  send_packet(packet);
}

void Lampify::turn_off() {
  ESP_LOGI(TAG, "Turning off lamp 0x%04X", device_id_);
  uint8_t packet[32];
  build_packet(0x11, 0x00, 0x00, packet);
  send_packet(packet);
}

void Lampify::set_level(uint8_t cold, uint8_t warm) {
  cold = clamp_level(cold);
  warm = clamp_level(warm);
  ESP_LOGI(TAG, "Setting lamp 0x%04X to cold=%d warm=%d", device_id_, cold, warm);
  uint8_t packet[32];
  build_packet(0x21, cold, warm, packet);
  send_packet(packet);
}

void Lampify::pair() {
  ESP_LOGI(TAG, "Pairing with lamp using ID 0x%04X", device_id_);
  uint8_t packet[32];
  uint8_t id_high = (device_id_ >> 8) & 0xFF;
  uint8_t id_low = device_id_ & 0xFF;
  build_packet(0x28, id_high, id_low, packet);
  send_packet(packet);
}

}  // namespace lampify
}  // namespace esphome
