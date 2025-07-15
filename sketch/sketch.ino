#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseESP8266.h>
#include <ArduinoOTA.h>

#include "tokens.h"
#include "webpage.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WebServer server(80);

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setupWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
}

void setupOTA() {
  ArduinoOTA.begin();
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
}

void handleRoot() {
  server.send_P(200, "text/html", WEB_page);
}

void handlePost() {
  if (server.hasArg("message")) {
    String message = server.arg("message");
    Firebase.setString(firebaseData, "/messages", message);
    server.send(200, "text/plain", "Posted!");
  } else {
    server.send(400, "text/plain", "Missing 'message'");
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();

  setupWiFi();
  setupOTA();
  setupFirebase();

  server.on("/", handleRoot);
  server.on("/post", handlePost);
  server.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}