#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WS2812FX.h>
#include <EEPROM.h>
#include "variables.cpp"

extern const char index_html[];
extern const char main_js[];
extern const char main_css[];

#define STATIC_IP
#ifdef STATIC_IP
IPAddress ip(192, 168, 0, 123);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

// QUICKFIX...See https://github.com/esp8266/Arduino/issues/263
// #define min(a, b) ((a) < (b) ? (a) : (b))
// #define max(a, b) ((a) > (b) ? (a) : (b))

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

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(16);

  delay(2500);
  Serial.println("\n\nStarting...");

  ws2812fx_setup();
  wifi_setup();
  server_setup();
  load_configs();

  Serial.println("Setup complete!");
}

void loop()
{
  unsigned long now = millis();

  server.handleClient();
  ws2812fx1.service();
  ws2812fx2.service();

  if (now - last_wifi_check_time > WIFI_TIMEOUT)
  {
    Serial.print("Checking WiFi... ");
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi connection lost. Reconnecting...");
      wifi_setup();
    }
    else
    {
      Serial.println("OK");
    }
    last_wifi_check_time = now;
  }
}

void wifi_setup()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);
#ifdef STATIC_IP
  WiFi.config(ip, gateway, subnet);
#endif

  unsigned long connect_start = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

    if (millis() - connect_start > WIFI_TIMEOUT)
    {
      Serial.println();
      Serial.print("Tried ");
      Serial.print(WIFI_TIMEOUT);
      Serial.print("ms. Resetting ESP now.");
      ESP.reset();
    }
  }

  Serial.println(" connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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
  for (uint8_t i = 0; i < server.args(); i++)
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
  }

  save_configs();
  server.send(200, "text/plain", "OK");
}
