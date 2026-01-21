# ESPHome Lampify Component

ESPHome external component for controlling Lampify-compatible BLE lamps.

**Status:** Working (almost perfect reliability)

## Installation

Add this to your ESPHome YAML:

```yaml
external_components:
  - source: github://steffen-heil-secforge/esphome-lampify
    components: [lampify]
```

## Usage

### Recommended Setup

```yaml
esp32_ble:
  enable_on_boot: true

# IMPORTANT: BLE tracker must be on-demand only to avoid interference
esp32_ble_tracker:
  scan_parameters:
    active: false
    continuous: false

lampify:

number:
  - platform: lampify
    name: "Lamp Device ID"

button:
  - platform: lampify
    name: "Pair Lamp"

light:
  - platform: lampify
    name: "Desk Lamp"

# Optional: diagnostic sensor showing last sent packet
text_sensor:
  - platform: lampify
    name: "Last Packet"
```

### IMPORTANT: BLE Scanner Interference

**Do NOT use `ble_scanner` text_sensor** - it triggers periodic scans that interfere with lamp control and cause intermittent failures:

```yaml
# DON'T DO THIS:
text_sensor:
  - platform: ble_scanner  # This causes lamp control to fail intermittently!
    name: "BLE Devices"
```

If you need BLE scanning, use manual scan buttons:
```yaml
button:
  - platform: template
    name: "Scan BLE"
    on_press:
      - esp32_ble_tracker.start_scan:
          continuous: false
  - platform: template
    name: "Stop BLE Scan"
    on_press:
      - esp32_ble_tracker.stop_scan:
```

This creates:
- A **number entity** to set the device ID from Home Assistant (persisted to flash)
- A **pair button** to pair with lamps
- A **light entity** with brightness and color temperature control

### With Preset Device ID

If you know your device ID, you can set it in YAML:

```yaml
esp32_ble:

lampify:
  device_id: 0x1234  # Optional: preset device ID

number:
  - platform: lampify
    name: "Lamp Device ID"

button:
  - platform: lampify
    name: "Pair Lamp"

light:
  - platform: lampify
    name: "Desk Lamp"
```

### With Manual Actions (for automations)

```yaml
esp32_ble:

lampify:
  id: my_lamp

# Automation example
button:
  - platform: template
    name: "Bright Mode"
    on_press:
      - lampify.set_level:
          id: my_lamp
          cold: 255
          warm: 255
```

## Pairing a New Lamp

1. Set the device ID (any value 0x0001-0xFFFF) via the number entity in Home Assistant
2. Power on the lamp
3. Within 5 seconds, press the "Pair Lamp" button in Home Assistant
4. The lamp will store the device ID and respond to future commands

## Entities

| Entity Type | Name | Description |
|-------------|------|-------------|
| `number` | Device ID | 0-65535, stored in flash |
| `button` | Pair | Sends pair command to lamp |
| `light` | Lamp | Brightness + color temperature control |

## Actions (for automations)

| Action | Parameters | Description |
|--------|------------|-------------|
| `lampify.turn_on` | none | Turn lamp on |
| `lampify.turn_off` | none | Turn lamp off |
| `lampify.set_level` | `cold`, `warm` (3-255) | Set brightness levels directly |
| `lampify.pair` | none | Pair with lamp |

## Hardware

Requires ESP32 with BLE support. Tested on ESP32-C3 Super Mini.

## Technical Details

### BLE Advertising
- **TX Power:** +9 dBm (maximum)
- **Advertising type:** `ADV_TYPE_IND` (connectable undirected)
- **Interval:** 0x0020 (20ms)
- **Reliability:** Sends 3 bursts per command (150ms each)

### Packet Protocol
- 32 bytes total with CRC16 checksum
- Bit reversal and BLE whitening applied
- Commands: `0x10` (on), `0x11` (off), `0x21` (set level), `0x28` (pair)

### Light Entity
- Color mode: Cold/Warm White (`COLD_WARM_WHITE`)
- Color temperature: 153-370 mireds (6500K-2700K)
- Minimum brightness level: 3 (for cold and warm individually)

## Troubleshooting

### Lamp not responding
1. Verify device ID is set (check number entity)
2. Ensure no BLE scan is running
3. Try multiple button presses
4. Check lamp is powered and in range

### Intermittent failures (works sometimes)
1. **Remove `ble_scanner` text_sensor** - most common cause
2. Set `esp32_ble_tracker` to `continuous: false` and `active: false`
3. Avoid triggering BLE scans while controlling lamp

### Device ID resets after OTA
The device ID is stored in flash but may reset after firmware updates. Re-enter it via the number entity.
