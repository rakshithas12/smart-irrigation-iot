# API reference

Local backend base URL: `http://localhost:5000`. All routes return JSON. No authentication is implemented. There are no query parameters. Vite proxies frontend `/api` requests to this backend in development.

## `GET /api/health`

Backend liveness. Example: `curl http://localhost:5000/api/health`

Response: `{"ok":true,"time":"<ISO-8601 timestamp>"}`

## `GET /api/sensors`

Returns latest reading. Example: `curl http://localhost:5000/api/sensors`. Response fields: `soilMoisture`, `temperature`, `humidity`, `pump`, `source`, and `timestamp`.

## `POST /api/sensors`

Adds/replaces latest reading and appends it to in-memory history. Required numeric fields: `soilMoisture`, `temperature`, `humidity`. Optional `pump` is used when boolean.

```sh
curl -X POST http://localhost:5000/api/sensors -H 'Content-Type: application/json' -d '{"soilMoisture":35,"temperature":28.5,"humidity":62,"pump":false}'
```

Success: `{"success":true,"data":<reading>}`. Invalid required fields return HTTP 400 with an `error` message. Numeric ranges are not validated. This generic HTTP interface does not establish that NodeMCU firmware is present.

## `GET /api/history`

Returns in-memory history. Example: `curl http://localhost:5000/api/history`. It starts with 20 generated items and is capped at 60 appended entries.

## `GET /api/dashboard`

Returns combined dashboard state. Example: `curl http://localhost:5000/api/dashboard`.

Shape: `{"sensors":<reading>,"mode":"AUTO|MANUAL","recommendation":{"status":...,"message":...,"level":...},"location":"..."}`.

## `GET /api/irrigation/recommendation`

Returns current recommendation `{status,message,level}` based on current soil moisture. Example: `curl http://localhost:5000/api/irrigation/recommendation`.

## `GET /api/pump/status`

Returns software state: `{"pump":<boolean>,"mode":"AUTO|MANUAL"}`. Example: `curl http://localhost:5000/api/pump/status`. It does not query physical hardware.

## `POST /api/pump/control`

Sets software pump state in MANUAL mode. Request: `{"state":true}` or `{"state":false}`.

```sh
curl -X POST http://localhost:5000/api/pump/control -H 'Content-Type: application/json' -d '{"state":true}'
```

Success: `{"success":true,"pump":<boolean>}`. AUTO mode or missing/non-boolean state returns HTTP 400 with an `error` message. No physical actuator is driven.

## `POST /api/mode`

Changes mode using `{"mode":"AUTO"}` or `{"mode":"MANUAL"}` (case-insensitive).

```sh
curl -X POST http://localhost:5000/api/mode -H 'Content-Type: application/json' -d '{"mode":"MANUAL"}'
```

Success: `{"success":true,"mode":"AUTO|MANUAL"}`. Invalid modes return HTTP 400. Switching to AUTO derives pump software state from the <30 threshold.

## `GET /api/weather`

Fetches Open-Meteo data using backend-configured coordinates. Example: `curl http://localhost:5000/api/weather`.

Success shape: `{"location":...,"current":...,"daily":...,"source":"Open-Meteo"}`. Upstream failure returns HTTP 503: `{"error":"Weather service unavailable"}`. The backend requests current temperature, humidity, precipitation, weather code, and wind speed, plus five forecast days.
