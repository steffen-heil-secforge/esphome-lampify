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

### Multi-Lamp Setup (Recommended)

Control up to 16 lamps from a single ESP32. Device IDs are configured in Home Assistant and persisted to flash.

```yaml
esp32_ble:
  enable_on_boot: true

# IMPORTANT: BLE tracker must be on-demand only to avoid interference
esp32_ble_tracker:
  scan_parameters:
    active: false
    continuous: false

lampify:
  lamps: 8  # Number of lamp slots (1-16, default: 8)

# Create entities for each lamp slot
number:
  - platform: lampify
    name: "Lamp 1 Device ID"
    lamp_index: 0
  - platform: lampify
    name: "Lamp 2 Device ID"
    lamp_index: 1
  # ... add more as needed

button:
  - platform: lampify
    name: "Pair Lamp 1"
    lamp_index: 0
  - platform: lampify
    name: "Pair Lamp 2"
    lamp_index: 1
  # ... add more as needed

light:
  - platform: lampify
    name: "Lamp 1"
    lamp_index: 0
  - platform: lampify
    name: "Lamp 2"
    lamp_index: 1
  # ... add more as needed

# Optional: diagnostic sensor showing last sent packet
text_sensor:
  - platform: lampify
    name: "Last Packet"
```

### Single Lamp Setup (Simple)

For controlling just one lamp, you can omit `lamp_index` (defaults to 0):

```yaml
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

### With Preset Device ID

If you know your device ID, you can set it in YAML (sets lamp 0):

```yaml
lampify:
  device_id: 0x1234  # Optional: preset device ID for lamp 0
```

### With Manual Actions (for automations)

```yaml
lampify:
  id: my_lampify

# Automation example
button:
  - platform: template
    name: "Bright Mode Lamp 2"
    on_press:
      - lampify.set_level:
          id: my_lampify
          lamp_index: 1
          cold: 255
          warm: 255
```

## Pairing a New Lamp

1. Set the device ID (any value 1-65535) via the number entity in Home Assistant
2. Power on the physical lamp
3. Within 5 seconds, press the corresponding "Pair Lamp" button in Home Assistant
4. The lamp will store the device ID and respond to future commands

**Note:** Device IDs are freely chosen numbers (not sequential). Use the same IDs as with the CLI tool.

## Configuration

### Lampify Component

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `lamps` | int | 8 | Number of lamp slots (1-16) |
| `device_id` | hex | - | Optional preset device ID for lamp 0 |

### Entity Options

All entities support the `lamp_index` option:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `lamp_index` | int | 0 | Which lamp slot (0-15) this entity controls |

## Entities

| Entity Type | Platform | Description |
|-------------|----------|-------------|
| `number` | lampify | Device ID input (0-65535), stored in flash |
| `button` | lampify | Pair button - sends pairing command |
| `light` | lampify | Light control with brightness and color temperature |
| `text_sensor` | lampify | Diagnostic - shows last sent packet (hex) |

## Actions (for automations)

| Action | Parameters | Description |
|--------|------------|-------------|
| `lampify.turn_on` | `lamp_index` (default: 0) | Turn lamp on |
| `lampify.turn_off` | `lamp_index` (default: 0) | Turn lamp off |
| `lampify.set_level` | `lamp_index`, `cold`, `warm` (3-255) | Set brightness levels directly |
| `lampify.pair` | `lamp_index` (default: 0) | Pair with lamp |

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

### Data Persistence
- Device IDs are stored in ESP32 NVS (Non-Volatile Storage)
- Survives OTA updates and reboots
- Only lost on full flash erase

## Troubleshooting

### Lamp not responding
1. Verify device ID is set (check number entity shows non-zero value)
2. Ensure no BLE scan is running
3. Try multiple button presses
4. Check lamp is powered and in range
5. Verify lamp was paired with this device ID

### Intermittent failures (works sometimes)
1. **Remove `ble_scanner` text_sensor** - most common cause
2. Set `esp32_ble_tracker` to `continuous: false` and `active: false`
3. Avoid triggering BLE scans while controlling lamp

### Device ID shows 0 in HA but lamp works
This was a bug in older versions where the number entity read the value before it was loaded from flash. Update to the latest version.
