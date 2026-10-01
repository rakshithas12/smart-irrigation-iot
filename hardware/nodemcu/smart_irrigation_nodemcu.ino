/*
 * NodeMCU ESP8266 sensor upload client for this repository's existing API.
 * This is not the lost original sketch. It implements only the upload path
 * supported by POST /api/sensors; it does not control the physical pump.
 * See ../README.md and docs/hardware.md before configuring or wiring.
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// ===== HARDWARE CONFIGURATION: replace sentinels with verified values =====
// Do not uncomment/use guessed pins. Verify these against physical wiring.
#define SOIL_MOISTURE_PIN -1  // Set to the wired NodeMCU analog input.
#define DHT_PIN -1            // Set to the wired DHT11 data pin.
#define SOIL_DRY_ADC -1       // Set from a measured dry sensor reading.
#define SOIL_WET_ADC -1       // Set from a measured wet sensor reading.

#if SOIL_MOISTURE_PIN < 0
#error "Set SOIL_MOISTURE_PIN to the verified analog input before compiling."
#endif
#if DHT_PIN < 0
#error "Set DHT_PIN to the verified DHT11 data pin before compiling."
#endif
#if SOIL_DRY_ADC < 0 || SOIL_WET_ADC < 0
#error "Set SOIL_DRY_ADC and SOIL_WET_ADC from calibration measurements before compiling."
#endif

#define DHT_TYPE DHT11

// Replace placeholders locally. Never commit real Wi-Fi credentials.
const char *WIFI_SSID = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char *SERVER_URL = "http://YOUR_SERVER_IP:5000/api/sensors";

// Configurable interval; the original device interval is unknown.
const unsigned long UPLOAD_INTERVAL_MS = 10000;
const unsigned long WIFI_RETRY_INTERVAL_MS = 10000;

DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastUploadMs = 0;
unsigned long lastWifiAttemptMs = 0;

bool isConfigured(const char *value) {
  return value[0] != '\0' && strstr(value, "YOUR_") == nullptr;
}

int soilPercentFromRaw(int raw) {
  // Endpoint order supports either sensor orientation; calibration is required.
  const long numerator = (long)(raw - SOIL_DRY_ADC) * 100L;
  const long denominator = (long)SOIL_WET_ADC - (long)SOIL_DRY_ADC;
  long percent = numerator / denominator;
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return (int)percent;
}

void connectWifiWhenDue() {
  if (WiFi.status() == WL_CONNECTED) return;

  const unsigned long now = millis();
  if (now - lastWifiAttemptMs < WIFI_RETRY_INTERVAL_MS) return;
  lastWifiAttemptMs = now;
  if (!isConfigured(WIFI_SSID) || !isConfigured(WIFI_PASSWORD)) {
    Serial.println(F("Set Wi-Fi placeholders before connecting."));
    return;
  }
  Serial.println(F("Wi-Fi disconnected; attempting reconnect."));
  if (WiFi.status() == WL_DISCONNECTED) WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  else WiFi.reconnect();
}

void uploadReading() {
  if (SOIL_DRY_ADC == SOIL_WET_ADC) {
    Serial.println(F("Invalid soil calibration: dry and wet endpoints must differ."));
    return;
  }

  const int soilRaw = analogRead(SOIL_MOISTURE_PIN);
  const int soilMoisture = soilPercentFromRaw(soilRaw);
  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();

  Serial.printf("Soil raw=%d, calibrated=%d%%, temperature=%.1f C, humidity=%.1f%%\n",
                soilRaw, soilMoisture, temperature, humidity);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(F("DHT11 read failed; skipping upload."));
    return;
  }
  if (!isConfigured(SERVER_URL)) {
    Serial.println(F("Set SERVER_URL to the backend host reachable on Wi-Fi."));
    return;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("No Wi-Fi; reading not uploaded."));
    return;
  }

  WiFiClient client;
  HTTPClient http;
  if (!http.begin(client, SERVER_URL)) {
    Serial.println(F("Could not initialize HTTP request."));
    return;
  }

  http.addHeader("Content-Type", "application/json");
  // The existing API accepts these three numeric fields. Omit pump so the
  // backend preserves its software pump flag; this firmware has no relay control.
  String body = F("{\"soilMoisture\":");
  body += soilMoisture;
  body += F(",\"temperature\":");
  body += String(temperature, 1);
  body += F(",\"humidity\":");
  body += String(humidity, 1);
  body += '}';

  const int status = http.POST(body);
  Serial.printf("POST /api/sensors returned HTTP %d\n", status);
  if (status > 0) Serial.println(http.getString());
  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println(F("Smart irrigation sensor uploader (prototype template)"));

  dht.begin();
  WiFi.mode(WIFI_STA);
  if (isConfigured(WIFI_SSID) && isConfigured(WIFI_PASSWORD)) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    lastWifiAttemptMs = millis();
  } else {
    Serial.println(F("Set Wi-Fi placeholders in the local sketch before use."));
  }

  if (SOIL_DRY_ADC == SOIL_WET_ADC)
    Serial.println(F("ERROR: soil dry/wet calibration endpoints are equal."));
}

void loop() {
  connectWifiWhenDue();
  const unsigned long now = millis();
  if (now - lastUploadMs >= UPLOAD_INTERVAL_MS) {
    lastUploadMs = now;
    uploadReading();
  }
  delay(10);
}
