# Hardware and firmware status

The project owner confirms the physical prototype contained a NodeMCU ESP8266, soil-moisture sensor, DHT11, relay module, DC pump, breadboard, and jumper wires. The original Arduino IDE `.ino` files are unavailable.

`nodemcu/smart_irrigation_nodemcu.ino` is a source-compatible sensor-upload starting point, not a recovered copy of the original firmware. It posts the existing backend's sensor JSON fields to `POST /api/sensors`. It intentionally does not drive the relay or pump: no pump command is sent by the backend to a device, and the relay pin/polarity are unknown.

## Before compiling or wiring

**VERIFY THESE PIN CONNECTIONS AGAINST THE PHYSICAL PROTOTYPE BEFORE UPLOADING.**

Set `SOIL_MOISTURE_PIN`, `DHT_PIN`, `SOIL_DRY_ADC`, and `SOIL_WET_ADC` in the firmware configuration. The source refuses to compile while the pin/calibration sentinels remain `-1`. These values must come from actual wiring and sensor measurements; none are guessed here. Also replace the Wi-Fi and server URL placeholders.

The soil percentage is a linear interpolation between measured dry and wet raw ADC endpoints. Verify the endpoint order and calibrate it with the actual sensor and soil. This is a configurable conversion, not a claim of scientific accuracy.

The relay and pump are confirmed components, but their circuit, electrical ratings, pin, and active-high/active-low behavior are unknown. Do not connect or control them from this firmware until verified separately. The existing API pump status/control is backend software state only.

## Arduino IDE

1. Install ESP8266 board support using the [official ESP8266 Arduino core instructions](https://arduino-esp8266.readthedocs.io/en/latest/installing.html).
2. Select the exact NodeMCU/ESP8266 board variant that matches the physical board; the variant is not identified here.
3. Select its connected COM port.
4. Install Adafruit's **DHT sensor library** and **Adafruit Unified Sensor** dependency through Library Manager ([official library notes](https://github.com/adafruit/DHT-sensor-library)). `ESP8266WiFi` and `ESP8266HTTPClient` come from the ESP8266 core; `ArduinoJson` is not used.
5. Edit the firmware configuration with verified pins, measured dry/wet values, Wi-Fi name/password, and backend URL. Never commit real Wi-Fi credentials.
6. Upload, then open Serial Monitor at 115200 baud.
7. Confirm Wi-Fi connection, finite DHT11 readings, calibrated soil percentage, and HTTP 200 from the backend.
8. Check the dashboard at `http://localhost:5173` (or the configured frontend host). The backend must be reachable from the NodeMCU at the configured server address; `localhost` on the microcontroller itself will not refer to the development computer.

This firmware does not verify relay operation or physical pump behavior.
