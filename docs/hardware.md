# Hardware documentation

The project owner confirms the physical prototype included a NodeMCU ESP8266, soil moisture sensor, DHT11, relay module, DC water pump, breadboard, and jumper wires. Those are user-provided prototype details; the checked-in application still has no original firmware, board setup, pin definitions, wiring diagram, or device communication implementation. This table distinguishes prototype information from what the repository source implements.

| Component | Purpose | Repository evidence |
|---|---|---|
| NodeMCU ESP8266 | Intended IoT controller | Confirmed by project owner; no original firmware or board-specific source is present. |
| Soil-moisture sensor | Soil measurement | Sensor is confirmed by project owner; exact model, output orientation, and calibration are unknown. API/UI expects a numeric value displayed as percent. |
| DHT11 | Temperature and humidity | Confirmed by project owner; API/UI has numeric temperature and humidity fields. No original sensor code is present. |
| Relay module | Switch pump load | Confirmed by project owner; GPIO mapping and active level are unknown. |
| DC water pump | Irrigation actuator | Confirmed by project owner; voltage/current and physical drive circuit are unknown. |
| Breadboard and jumper wires | Prototype wiring | Confirmed by project owner; actual wiring map was not provided. |

## Missing information

- NodeMCU variant and firmware source
- Soil sensor model, output orientation, and calibration endpoints
- GPIO assignments and wiring
- Relay module and pump electrical specifications
- Confirmation of device-to-backend transport and server address used by the original prototype
- Power and isolation design

Pin mapping should be verified against the physical prototype. The existing API's pump state does not drive hardware. Add the original or physically verified firmware, connection diagram, and component specifications when available; do not commit Wi-Fi credentials or other secrets. No separate Arduino board is established by the available information; Arduino IDE is the programming tool, not evidence of a second board.

## Included NodeMCU template

`hardware/nodemcu/smart_irrigation_nodemcu.ino` uses the existing `POST /api/sensors` API. It sends `soilMoisture` as a configured linear percentage and sends DHT11 `temperature` in °C and `humidity` in percent. The backend requires the three values to be JSON numbers. The template omits `pump` because that field only updates backend software state and is not a device command. It does not control the relay.

Configure and calibrate before uploading:

1. Verify the NodeMCU analog input and DHT11 data pin from the actual wiring; set `SOIL_MOISTURE_PIN` and `DHT_PIN` in the sketch.
2. Measure the raw soil-sensor ADC values for known dry and wet conditions; set `SOIL_DRY_ADC` and `SOIL_WET_ADC`. The template maps those measured endpoints linearly to 0% and 100%; confirm the sensor orientation and dashboard's interpretation against the prototype.
3. Set local Wi-Fi name/password and a backend URL reachable from the same Wi-Fi network, for example `http://<computer-LAN-IP>:5000/api/sensors`. Do not use `localhost` as the NodeMCU's server address.
4. Install the ESP8266 board package in Arduino IDE. The current ESP8266 core installation guide is [here](https://arduino-esp8266.readthedocs.io/en/latest/installing.html). Choose the board variant matching the physical NodeMCU and its COM port; exact board selection cannot be inferred from the repository.
5. Install Adafruit's **DHT sensor library** and **Adafruit Unified Sensor** dependency through Library Manager ([library and dependency notes](https://github.com/adafruit/DHT-sensor-library)). The sketch uses the ESP8266 core's Wi-Fi/HTTP libraries; it does not use ArduinoJson.
6. Upload and open Serial Monitor at 115200 baud. Confirm Wi-Fi, finite DHT values, soil raw/calibrated readings, and successful HTTP status.
7. Confirm the dashboard updates. The backend mock timer continues and can replace the uploaded latest reading after five seconds.

The template's 10-second upload interval is configurable and is not asserted to match the original prototype. It deliberately blocks compilation until pins and calibration are set.

## Relay, pump, and power

The project owner confirms relay and DC pump components, but no exact relay module, supply ratings, wiring, or active-high/active-low behavior is available. Do not connect the pump from a GPIO pin or use this template as a pump driver. Verify the relay's logic and ratings, pump supply, grounding/isolation, and fail-safe state against the physical hardware design before implementing actuator control. The checked-in backend cannot send pump commands to the NodeMCU.
