# Data flow

This describes checked-in behavior and distinguishes the new upload template from the unavailable original device firmware.

1. **Sensor measurement:** The backend initializes mock soil-moisture, temperature, and humidity values and generates new values every five seconds. A new NodeMCU sketch template reads a soil analog input and DHT11 after physical pin/calibration settings are supplied. This is not the lost original sketch.
2. **NodeMCU processing:** The project owner identifies NodeMCU ESP8266 as part of the prototype. The template computes a configurable linear soil-moisture percentage from measured dry/wet ADC endpoints; actual endpoints and pin assignments remain unknown and must be verified.
3. **Data transmission:** The template posts numeric `soilMoisture`, `temperature`, and `humidity` JSON fields to existing `POST /api/sensors` over HTTP. This endpoint accepts the fields. The original device transport is unknown. The backend mock generator continues and may overwrite submitted values on its next cycle.
4. **Backend processing:** Express stores the latest reading, operating mode, and up to 60 history entries in process memory. Startup creates 20 generated history samples. There is no database or durable storage.
5. **Dashboard update:** React requests dashboard data and history immediately and every three seconds. Vite proxies frontend `/api` requests to the local backend during development.
6. **Weather:** The backend calls Open-Meteo using configured coordinates and returns current plus five-day daily forecast data. The frontend requests it at startup and every 15 minutes.
7. **Recommendation and pump state:** The backend recommendation uses soil thresholds (<30, <45, otherwise). In AUTO mode, backend state sets pump true below 30 and false otherwise. In MANUAL mode, the user can set the software pump flag through API calls.
8. **Physical actuation boundary:** The prototype includes relay and pump per the owner, but no relay/pump driver exists in the backend or template. The API has no device command channel; pump-state changes do not establish physical pump operation. Relay pin and active level remain unknown.

```text
Backend mock timer ─────────┐
                            ├─> Express in-memory state/API ─> React dashboard
NodeMCU upload template ──────┘                │
                                               └─> Open-Meteo weather request

Original NodeMCU firmware/device link: unavailable; template is not verified on hardware
Physical pump/relay output: not included
```
