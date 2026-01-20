# ESPHome Lampify Component

ESPHome external component for controlling Lampify-compatible BLE lamps.

## Installation

Add this to your ESPHome YAML:

```yaml
external_components:
  - source: github://steffen-heil-secforge/esphome-lampify
    components: [lampify]
```

## Usage

### Minimal Setup (Configure from Home Assistant)

```yaml
esp32_ble:

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

Requires ESP32 with BLE support.
