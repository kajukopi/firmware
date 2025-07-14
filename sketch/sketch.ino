#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "tokens.h"
#include "webpage.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

Servo servo;
int relayPin = D6;
int ledPin = D7;

void displayStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.localIP());

  int rssi = WiFi.RSSI();
  lcd.setCursor(0, 1);
  lcd.print("Signal: ");
  lcd.print(rssi);
  lcd.print(" dBm");
}

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleCommand() {
  String cmd = server.arg("cmd");
  if (cmd == "servo0") servo.write(0);
  else if (cmd == "servo90") servo.write(90);
  else if (cmd == "servo180") servo.write(180);
  else if (cmd == "relay") digitalWrite(relayPin, !digitalRead(relayPin));
  else if (cmd == "led") digitalWrite(ledPin, !digitalRead(ledPin));

  server.send(200, "text/plain", "OK");
}

void setup() {
  lcd.begin();
  lcd.backlight();
  servo.attach(D5);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0, 0); lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 1); lcd.print(".");
  }

  displayStatus();

  server.on("/", handleRoot);
  server.on("/cmd", handleCommand);
  httpUpdater.setup(&server, "/update");

  server.begin();
}

void loop() {
  server.handleClient();
}