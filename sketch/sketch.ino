#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "tokens.h"
#include "webpage.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPUpdateServer.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat LCD standar 0x27
AsyncWebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  Serial.begin(115200);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());

  // OTA via webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
  });

  // OTA firmware update
  httpUpdater.setup(&server);

  // LCD message handler
  server.on("/lcd", HTTP_POST, [](AsyncWebServerRequest *request){
    String msg;
    if (request->hasParam("msg", true)) {
      msg = request->getParam("msg", true)->value();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(msg.substring(0,16));
      if(msg.length() > 16) {
        lcd.setCursor(0,1);
        lcd.print(msg.substring(16,32));
      }
    }
    request->redirect("/");
  });

  server.begin();
}

void loop() {
  // nothing to do
}