# ESPHome Lampify Component

ESPHome external component for controlling Lampify-compatible BLE lamps.

## Installation

Add this to your ESPHome YAML:

```yaml
external_components:
  - source: github://yourusername/esphome-lampify
    components: [lampify]
```

## Usage

### As a Light Entity (Recommended)

```yaml
esp32_ble:

light:
  - platform: lampify
    name: "Desk Lamp"
    device_id: 0x1234
```

This creates a light with brightness and color temperature control in Home Assistant.

### With Manual Actions

```yaml
esp32_ble:

lampify:
  id: my_lamp
  device_id: 0x1234

button:
  - platform: template
    name: "Lamp On"
    on_press:
      - lampify.turn_on:
          id: my_lamp

  - platform: template
    name: "Lamp Off"
    on_press:
      - lampify.turn_off:
          id: my_lamp

  - platform: template
    name: "Lamp Bright"
    on_press:
      - lampify.set_level:
          id: my_lamp
          cold: 255
          warm: 255

  - platform: template
    name: "Pair Lamp"
    on_press:
      - lampify.pair:
          id: my_lamp
```

## Pairing

To pair with a new lamp:

1. Power on the lamp (within 5 seconds of power-on)
2. Trigger the `lampify.pair` action

The lamp will store the device ID and only respond to commands with matching IDs.

## Configuration

| Option | Type | Required | Description |
|--------|------|----------|-------------|
| `device_id` | hex | Yes | 16-bit hex ID (0x0000-0xFFFF) |
| `name` | string | Yes (light) | Entity name in Home Assistant |

## Actions

| Action | Parameters | Description |
|--------|------------|-------------|
| `lampify.turn_on` | none | Turn lamp on |
| `lampify.turn_off` | none | Turn lamp off |
| `lampify.set_level` | `cold`, `warm` (3-255) | Set brightness levels |
| `lampify.pair` | none | Pair with lamp |

## Hardware

Requires ESP32 with BLE support.
