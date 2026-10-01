# Development setup

## Prerequisites

- Node.js 18 or newer
- npm
- Network access to install dependencies; Open-Meteo access is needed for weather display

## Install and start

Run from the project root:

```sh
npm run install:all
npm run dev
```

The frontend runs at `http://localhost:5173`; the backend runs at `http://localhost:5000`. Vite forwards API calls from the frontend to the backend.

## Configuration

Copy `backend/.env.example` to `backend/.env` to override backend settings:

- `PORT`: backend listening port (default `5000`)
- `LATITUDE`, `LONGITUDE`: Open-Meteo forecast coordinates (defaults point to Bengaluru)
- `LOCATION_NAME`: displayed location name (default `Bengaluru`)

No API key or secret is required by the current implementation. Keep local `.env` files out of version control.

## Build

```sh
npm --prefix frontend run build
```

## Mock versus external readings

The backend starts with generated mock values and keeps generating them every five seconds. The repository includes a NodeMCU sensor-upload template that posts JSON readings to `POST /api/sensors`; it is not the original firmware and requires verified pin/calibration and network settings. Since mock generation remains active, it may overwrite submitted values on the next tick.
