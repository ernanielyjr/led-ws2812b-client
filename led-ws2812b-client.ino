#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WS2812FX.h>
#include <EEPROM.h>
#include "variables.cpp"

extern const char index_html[];
extern const char main_js[];
extern const char main_css[];

// #define DEVICE_NAME "your-name"
// #define DEVICE_PASS "your-pass"

// TODO: customize WifiManager UI
// TODO: split in more files
// TODO: improve WS2812FX segments

// TODO: custom startup effect
// TODO: custom red/blue police effect

// QUICKFIX...See https://github.com/esp8266/Arduino/issues/263
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define LED_PIN_1 0
#define LED_PIN_2 2
#define LED_COUNT 142

#define WIFI_TIMEOUT 30000
#define HTTP_PORT 80

unsigned long last_wifi_check_time = 0;
uint8_t myEffects[] = {}; // Criar modo policia

uint32_t effect = FX_MODE_RAINBOW_CYCLE;
uint32_t color = 0xFF5900;
uint32_t speed = 1000;
uint32_t brightness = 128;

const uint8_t SAVE_ADDR_EFFECT = 0;
const uint8_t SAVE_ADDR_COLOR = 4;
const uint8_t SAVE_ADDR_SPEED = 8;
const uint8_t SAVE_ADDR_BRIGHTNESS = 12;

WiFiManager wifiManager;

WS2812FX ws2812fx1 = WS2812FX(LED_COUNT, LED_PIN_1, NEO_GRB + NEO_KHZ800);
WS2812FX ws2812fx2 = WS2812FX(LED_COUNT, LED_PIN_2, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(HTTP_PORT);

uint32_t EEPROMReadlong(uint32_t address)
{
  uint8_t data4 = EEPROM.read(address);
  uint8_t data3 = EEPROM.read(address + 1);
  uint8_t data2 = EEPROM.read(address + 2);
  uint8_t data1 = EEPROM.read(address + 3);

  return ((data4 << 0) & 0xFF) + ((data3 << 8) & 0xFFFF) + ((data2 << 16) & 0xFFFFFF) + ((data1 << 24) & 0xFFFFFFFF);
}

void EEPROMWritelong(uint32_t address, uint32_t value)
{
  byte data4 = (value & 0xFF);
  byte data3 = ((value >> 8) & 0xFF);
  byte data2 = ((value >> 16) & 0xFF);
  byte data1 = ((value >> 24) & 0xFF);

  EEPROM.write(address, data4);
  EEPROM.write(address + 1, data3);
  EEPROM.write(address + 2, data2);
  EEPROM.write(address + 3, data1);
}

void save_configs()
{
  bool has_changes = false;
  // TODO: fazer debounce de 10 segundos

  if (EEPROMReadlong(SAVE_ADDR_EFFECT) != effect)
  {
    EEPROMWritelong(SAVE_ADDR_EFFECT, effect);
    has_changes = true;
  }

  if (EEPROMReadlong(SAVE_ADDR_COLOR) != color)
  {
    EEPROMWritelong(SAVE_ADDR_COLOR, color);
    has_changes = true;
  }

  if (EEPROMReadlong(SAVE_ADDR_SPEED) != speed)
  {
    EEPROMWritelong(SAVE_ADDR_SPEED, speed);
    has_changes = true;
  }

  if (EEPROMReadlong(SAVE_ADDR_BRIGHTNESS) != brightness)
  {
    EEPROMWritelong(SAVE_ADDR_BRIGHTNESS, brightness);
    has_changes = true;
  }

  if (has_changes)
  {
    EEPROM.commit();
    Serial.println("Configs saved!");
  }
}

void load_configs()
{
  setEffect(EEPROMReadlong(SAVE_ADDR_EFFECT));
  setColor(EEPROMReadlong(SAVE_ADDR_COLOR));
  setSpeed(EEPROMReadlong(SAVE_ADDR_SPEED));
  setBrightness(EEPROMReadlong(SAVE_ADDR_BRIGHTNESS));

  Serial.println("Configs loaded!");
}

void server_setup()
{
  server.on("/", srv_handle_index_html);
  server.on("/main.js", srv_handle_main_js);
  server.on("/main.css", srv_handle_main_css);

  server.on("/config", HTTP_GET, srv_handle_config_get);
  server.on("/config", HTTP_POST, srv_handle_config_post);

  server.onNotFound(srv_handle_not_found);
  server.begin();
  Serial.println("HTTP server started!");
}

void ws2812fx_setup()
{
  ws2812fx1.init();
  ws2812fx1.start();
  ws2812fx2.init();
  ws2812fx2.start();
  Serial.println("WS2812FX started!");
}

void wifi_manager_setup()
{
  wifiManager.autoConnect(DEVICE_NAME, DEVICE_PASS);
  Serial.println("Wifi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void ota_setup()
{
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.setPassword(DEVICE_PASS);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(16);

  delay(2500);
  Serial.println("\n\nStarting...");

  wifi_manager_setup();
  ota_setup();
  ws2812fx_setup();
  server_setup();
  load_configs();

  Serial.println("Setup complete!");
}

void loop()
{
  ArduinoOTA.handle();

  unsigned long now = millis();

  server.handleClient();
  ws2812fx1.service();
  ws2812fx2.service();
}

void srv_handle_not_found()
{
  server.send(404, "text/plain", "File Not Found");
}

void srv_handle_index_html()
{
  server.send_P(200, "text/html", index_html);
}

void srv_handle_main_js()
{
  server.send_P(200, "application/javascript", main_js);
}

void srv_handle_main_css()
{
  server.send_P(200, "text/css", main_css);
}

void setColor(uint32_t _color)
{
  color = _color;
  ws2812fx1.setColor(color);
  ws2812fx2.setColor(color);
  Serial.print("setColor ");
  Serial.println(ws2812fx1.getColor());
}

void setEffect(uint32_t _effect)
{
  effect = _effect;
  ws2812fx1.setMode(effect % ws2812fx1.getModeCount());
  ws2812fx2.setMode(effect % ws2812fx2.getModeCount());
  Serial.print("setEffect ");
  Serial.println(ws2812fx1.getModeName(ws2812fx1.getMode()));
}

void setSpeed(uint32_t _speed)
{
  speed = _speed;
  ws2812fx1.setSpeed(speed);
  ws2812fx2.setSpeed(speed);
  Serial.print("setSpeed ");
  Serial.println(ws2812fx1.getSpeed());
}

void setBrightness(uint32_t _brightness)
{
  brightness = _brightness;
  ws2812fx1.setBrightness(brightness);
  ws2812fx2.setBrightness(brightness);
  Serial.print("setBrightness ");
  Serial.println(ws2812fx1.getBrightness());
}

void srv_handle_config_get()
{
  String json = "{";

  json += "\"effect\":";
  json += String(effect, DEC);
  json += ",";

  json += "\"color\": \"#";
  json += String(color, HEX);
  json += "\",";

  json += "\"brightness\":";
  json += String(brightness, DEC);
  json += ",";

  json += "\"speed\":";
  json += String(speed, DEC);
  json += ",";

  json += "\"effects\":[";

  uint8_t num_effects = sizeof(myEffects) > 0 ? sizeof(myEffects) : ws2812fx1.getModeCount();
  for (uint8_t i = 0; i < num_effects; i++)
  {
    uint8_t m = sizeof(myEffects) > 0 ? myEffects[i] : i;

    json += "{";
    json += "\"id\":";
    json += i;
    json += ",\"name\":\"";
    json += ws2812fx1.getModeName(m);
    json += "\"}";

    if (i < num_effects - 1)
    {
      json += ",";
    }
  }

  json += "]";
  json += "}";

  server.send(200, "application/json", json);
}

void srv_handle_config_post()
{
  for (uint8_t i = 0; i < server.args(); i++) // TODO: get args from body of post
  {
    const String parametter = server.argName(i);
    const uint32_t value = strtol(server.arg(i).c_str(), NULL, 10);

    if (parametter == "color" && value != color)
    {
      setColor(value);
    }
    if (parametter == "effect" && value != effect)
    {
      setEffect(value);
    }
    if (parametter == "speed" && value != speed)
    {
      setSpeed(value);
    }
    if (parametter == "brightness" && value != brightness)
    {
      setBrightness(value);
    }
    if (parametter == "restart" && value == 1)
    {
      esp_restart();
      return;
    }
    if (parametter == "wifi-reset" && value == 1)
    {
      reset_wifi();
      return;
    }
  }

  save_configs();
  server.send(200, "text/plain", "OK");
}

void esp_restart()
{
  server.send(204);
  delay(500);
  ESP.restart();
}

void reset_wifi()
{
  server.send(204);
  delay(500);
  wifiManager.resetSettings();
  delay(500);
  ESP.restart();
}