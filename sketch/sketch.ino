#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "lcd.h"
#include "webpage.h"

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

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
  return String(rssi) + " dBm";
}

void setup() {
  Serial.begin(115200);
  lcdInit();
  lcdShowStartup();

  lcdShowConnecting();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lcdShowSearching();
    delay(1000);
  }

  currentIp = WiFi.localIP().toString();
  addLog("✅ WiFi Connected! IP: " + currentIp);
  lcdShowIp(currentIp);

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
    lcdShowTempMessage("OTA Update Start");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print((progress * 100) / total);
    lcd.print("%   ");
  });
  ArduinoOTA.onEnd([]() {
    lcdShowTempMessage("OTA Update Done");
    delay(2000);
    lcdShowIp(currentIp);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    lcdShowTempMessage("OTA Error:", String(error));
  });
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();

  if (lcdRestoreTime > 0 && millis() > lcdRestoreTime) {
    lcdRestoreTime = 0;
    lcdShowIp(currentIp);
  }

  if (lcdRestoreTime == 0 && millis() - lastStatusDisplay > 10000) {
    lastStatusDisplay = millis();
    String rssiStr = getSignalStrength();
    lcdShowSignalStrength(rssiStr);
  }
}