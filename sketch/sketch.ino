#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "webpage.h"

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lcdRestoreTime = 0;
unsigned long lastStatusDisplay = 0;

String logBuffer = "";
String currentIp = "";

void addLog(String msg) {
  Serial.println(msg);
  logBuffer += msg + "\n";
  if (logBuffer.length() > 1500)
    logBuffer = logBuffer.substring(logBuffer.length() - 1500);
}

String getSignalStrength() {
  int rssi = WiFi.RSSI();
  String bar = "";
  char block = (char)255; // karakter blok penuh

  if (rssi > -50)       bar = String(block) + block + block + block + block;
  else if (rssi > -60)  bar = String(block) + block + block + block;
  else if (rssi > -70)  bar = String(block) + block + block;
  else if (rssi > -80)  bar = String(block) + block;
  else if (rssi > -90)  bar = String(block);
  else                  bar = "-----"; // tidak ada sinyal

  return String(rssi) + " dBm (" + bar + ")";
}

void displayIp() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print(currentIp);
}

void showTempMessage(String line1, String line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  lcdRestoreTime = millis() + 3000;
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("ESP8266");
  lcd.setCursor(0, 1); lcd.print("Web Server");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 1); lcd.print("Searching...");
    delay(1000);
  }

  currentIp = WiFi.localIP().toString();
  addLog("✅ WiFi Connected! IP: " + currentIp);
  displayIp();

  server.on("/", []() {
    server.send_P(200, "text/html", WEB_page);
  });

  server.on("/status", HTTP_GET, []() {
    String json = "{\"ip\":\"" + WiFi.localIP().toString() + "\",\"signal\":\"" + getSignalStrength() + "\"}";
    server.send(200, "application/json", json);
  });

  server.on("/log", HTTP_GET, []() {
    server.send(200, "text/plain", logBuffer);
  });

  httpUpdater.setup(&server, "/update");
  server.begin();

  ArduinoOTA.onStart([]() {
    showTempMessage("OTA Update Start");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print((progress * 100) / total);
    lcd.print("%   ");
  });
  ArduinoOTA.onEnd([]() {
    showTempMessage("OTA Update Done");
    delay(2000);
    displayIp();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    showTempMessage("OTA Error:", String(error));
  });
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();

  if (lcdRestoreTime > 0 && millis() > lcdRestoreTime) {
    lcdRestoreTime = 0;
    displayIp();
  }

  if (lcdRestoreTime == 0 && millis() - lastStatusDisplay > 10000) {
    lastStatusDisplay = millis();
    String rssiStr = getSignalStrength();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Signal Strength");
    lcd.setCursor(0, 1);
    lcd.print(rssiStr.substring(0, 16));
    lcdRestoreTime = millis() + 2000;
  }
}