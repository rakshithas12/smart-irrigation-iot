# IoT-Based Smart Irrigation and Weather Monitoring System

A web dashboard prototype and Express API for soil readings, weather information, and irrigation mode/pump-state monitoring and control.

## Project Overview

This academic IoT project is intended to monitor soil moisture and environmental conditions and present irrigation information in a web interface. The repository implements the dashboard and backend prototype. It generates simulated sensor readings by default and accepts JSON readings over HTTP. A NodeMCU sensor-upload template is now included, but it is not the lost original firmware and does not demonstrate a running device connection until the physical pins, calibration, Wi-Fi, and server address are configured and verified.

The backend retrieves weather forecasts from Open-Meteo. Sensor values, operating mode, and history are held in memory and reset when the backend restarts.

## Problem Statement

Irrigation decisions need information about soil conditions and weather. This project provides a dashboard prototype where those categories of information can be viewed together and where a pump state can be managed through AUTO/MANUAL software controls. No measured field results or water savings are included in the repository.

## Objectives

- Present soil moisture, temperature, humidity, and pump state in a browser dashboard.
- Show recent readings and a threshold-based irrigation recommendation.
- Provide AUTO and MANUAL software modes for pump-state control.
- Display current and forecast weather from Open-Meteo.
- Provide an HTTP API for reading data and accepting external sensor readings.

## Implemented Features

- Responsive single-page React dashboard with status and refresh controls.
- Soil moisture, temperature, humidity, and pump-state cards.
- Recent sensor-history chart for soil moisture, temperature, and humidity.
- AUTO and MANUAL mode selection, with manual pump-state controls.
- Irrigation recommendation based on soil-moisture thresholds.
- Current conditions and five-day daily forecast from Open-Meteo.
- Backend-generated mock readings and an HTTP sensor-ingestion endpoint.

## System Architecture

```text
Default repository behavior:
Backend mock generator (5-second interval)
        -> in-memory readings and history
        -> Express REST API (port 5000)
        -> React/Vite dashboard (polls every 3 seconds)

Optional sensor upload path (included template; physical configuration required):
NodeMCU ESP8266 + soil sensor + DHT11 -> Wi-Fi HTTP POST /api/sensors
                                      -> Express in-memory state -> dashboard

Weather path:
Dashboard -> GET /api/weather -> Express -> Open-Meteo -> dashboard

User controls:
Dashboard -> POST /api/mode or /api/pump/control -> Express software state
```

The repository does not contain the original NodeMCU firmware, a database, or a physical pump/relay driver. The included sketch uses the backend's existing HTTP sensor endpoint; it is a newly reconstructed upload template, not proof of the original device protocol. See [architecture](docs/architecture.md) and [data flow](docs/data-flow.md).

## Hardware Components

The project owner confirms the physical prototype contains NodeMCU ESP8266, a soil-moisture sensor, DHT11, relay module, DC water pump, breadboard, and jumper wires. The original firmware and wiring details are unavailable. The dashboard/API fields do not verify physical connections. See [hardware notes](docs/hardware.md).

The UI's “DHT11” and “Arduino/NodeMCU” labels are display strings; they do not establish a device link or relay control in the backend.

Pin mapping should be verified against the physical prototype.

## Software Technologies

- Frontend: React 18, Vite 6, Recharts, Lucide React.
- Backend: Node.js ES modules, Express 4, CORS, dotenv.
- External weather service: Open-Meteo forecast API.
- Runtime state: in-memory JavaScript objects and arrays; no database.

## IoT Data Flow

The backend generates readings by default. The included NodeMCU template can submit readings as JSON to `POST /api/sensors` after its compile-time hardware settings are verified; the original device communication is unknown. The dashboard polls the backend for data. Weather travels separately through the backend to Open-Meteo. Detailed stages are in [data-flow.md](docs/data-flow.md).

## Dashboard

The single-page dashboard displays API online/offline status, sensor metric cards, a recent history chart, pump state and mode, irrigation recommendation, weather conditions and forecast, last update, and the backend reading source. Weather can show as unavailable when the upstream request fails.

### Dashboard Preview

Screenshots can be added here. No screenshot or hardware photograph was present in the inspected repository.

## Soil Moisture and Irrigation Recommendation

The backend returns `IRRIGATION REQUIRED` below 30, `MONITOR` below 45, and `SOIL MOISTURE OK` otherwise. These thresholds apply to the numeric reading; calibration and units are not documented beyond the UI's `%` label.

In AUTO mode, backend software state sets the pump flag on below 30 and off otherwise. In MANUAL mode, the dashboard allows the user to set that flag. Neither path is connected to a physical pump in this repository.

## Weather Monitoring

The backend requests current and daily forecast data from Open-Meteo using `LATITUDE`, `LONGITUDE`, and `LOCATION_NAME` configuration. Defaults target Bengaluru. The frontend refreshes weather at startup and every 15 minutes. No weather sensor hardware is read by this code.

## Backend and API

The Express backend listens on port 5000 by default. It serves sensor, history, dashboard, recommendation, pump, mode, health, and weather routes. See the [API reference](docs/api.md) for methods, request examples, responses, and errors.

## Installation and Configuration

Requirements: Node.js 18+ and npm. From the repository root:

```sh
npm run install:all
```

Optional backend configuration: copy `backend/.env.example` to `backend/.env` and edit as needed. Available variables are `PORT`, `LATITUDE`, `LONGITUDE`, and `LOCATION_NAME`. No API key or Wi-Fi credentials are required by the current implementation. Local `.env` files are ignored by Git.

## Running the Project

Start both services from the root:

```sh
npm run dev
```

Open `http://localhost:5173`. Vite proxies `/api` to `http://localhost:5000`. The backend creates mock values automatically. To build the frontend bundle:

```sh
npm --prefix frontend run build
```

For setup details and a sensor-ingestion example, see [setup.md](docs/setup.md).

## Hardware Setup

Consult [hardware.md](docs/hardware.md) and physical prototype documentation before wiring components. Current pump buttons change API state only; they do not operate a relay or pump. Pin mapping should be verified against the physical prototype.

## Firmware

The lost original `.ino` file cannot be recovered from this repository. [The NodeMCU sketch](hardware/nodemcu/smart_irrigation_nodemcu.ino) is a clearly marked sensor-upload template for the existing `POST /api/sensors` endpoint. It reads soil analog input and DHT11 data and sends the API's numeric JSON fields. It intentionally does not control the relay or pump because the backend has no device command channel and the relay pin/polarity are unknown.

The sketch deliberately fails compilation until verified pins and soil calibration endpoints are entered. Follow [hardware/README.md](hardware/README.md) for Arduino IDE prerequisites, Wi-Fi/server configuration, calibration, and upload checks. No separate Arduino firmware is included because a second Arduino board is not established; Arduino IDE is the upload tool.

## Project Structure

```text
backend/        Express server, package metadata, environment example
frontend/       React/Vite dashboard and source
hardware/       NodeMCU sensor-upload template and hardware notes
docs/           API, architecture, setup, hardware, and data-flow notes
README.md      Project overview and run instructions
.gitignore     Ignores dependencies, builds, local env files, and editor artifacts
package.json   Root install/development scripts
```

## Results and Demonstration

The checked-in application can be run locally with generated demo readings and an Open-Meteo connection. No measured experimental results, hardware demonstration, or screenshots are included in this repository.

## Limitations

- Original NodeMCU firmware and verified device communication are absent; the included upload template requires physical pin/calibration configuration.
- Readings and history are in-memory and reset on backend restart.
- The mock generator continues while the server runs and can overwrite submitted sensor values.
- Pump mode and state are software-only; no physical actuator driver is present.
- Sensor units/calibration, exact models, GPIO pins, wiring, and power design are unspecified.
- No authentication, authorization, persistence, or rate limiting is implemented; CORS allows all origins.
- No automated test suite is present.

## Troubleshooting

- If the dashboard remains on its startup screen, make sure the backend is running on port 5000.
- If weather is unavailable, check network access to Open-Meteo and configured coordinates.
- If the frontend cannot reach the API, start both services with `npm run dev` from the root and keep the Vite `/api` proxy configuration.
- Manual pump controls are disabled in AUTO mode by design.

## Contributing

Preserve the distinction between implemented code and planned hardware integration. Document and verify board, sensor, pin, protocol, and actuator changes against the physical prototype. Do not commit credentials or local `.env` files.

## License

No license has been selected yet. See the repository owner's chosen license file once added.
