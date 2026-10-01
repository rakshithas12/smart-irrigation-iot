# Architecture notes

## Components

- `frontend/` contains one React page, styling, Vite configuration, and the browser entry point.
- `backend/server.js` contains the Express REST API, mock reading generator, in-memory history, recommendation rule, pump/mode state, and Open-Meteo proxy.
- `backend/.env.example` lists `PORT`, `LATITUDE`, `LONGITUDE`, and `LOCATION_NAME` defaults.

## Data flow

```text
Prototype components confirmed by owner:
Soil sensor + DHT11 -> NodeMCU ESP8266 -> Wi-Fi / HTTP upload template
                                              -> POST /api/sensors
                                              -> Express in-memory state
                                              -> React dashboard

Weather: Express -> Open-Meteo -> React dashboard
Pump: dashboard -> Express software state only
Physical relay/pump command path: not implemented
```

The backend initializes a mock reading and 20 generated history samples at startup. A timer updates mock sensor values every five seconds and retains at most 60 history entries. The dashboard requests `/api/dashboard` and `/api/history` on startup and every three seconds. It requests weather at startup and every 15 minutes.

`POST /api/sensors` can replace the latest sensor reading and append it to memory history. The included NodeMCU sketch is a newly created HTTP upload template for this endpoint; it is not the lost original device firmware and requires verified hardware configuration. The UI then identifies that latest value as `hardware`. The running mock timer is not stopped by sensor submissions, so the latest reading may revert to mock data on its next tick.

The recommendation is derived from `soilMoisture`: below 30 yields `IRRIGATION REQUIRED`; from 30 through 44 yields `MONITOR`; 45 or greater yields `SOIL MOISTURE OK`. In AUTO mode the pump state is true below 30 and false otherwise. MANUAL mode permits API pump-state updates.

Weather is fetched by the backend from Open-Meteo using configured latitude and longitude. The backend returns current conditions and daily forecast data to the browser.

## Storage and security

All sensor values, history, and mode are in-memory variables. There is no database or persistence. No login or API authentication is implemented. Express CORS is enabled for all origins. Treat the service as a local prototype and do not expose it to the public internet without adding appropriate security controls.

## Hardware boundary

The project owner confirms the physical prototype components as NodeMCU ESP8266, soil-moisture sensor, DHT11, relay module, DC pump, breadboard, and jumper wires. The repository contains no original microcontroller sketch, wiring diagram, or actuator driver. The added template posts to the existing `POST /api/sensors` route, but this does not establish that HTTP was the original device transport. The backend pump endpoints manipulate software state only and do not send a command to the NodeMCU or relay.

No separate Arduino board is established; Arduino IDE is identified as the firmware development tool. See [hardware.md](hardware.md) for confirmed prototype parts and missing pin/calibration details.
