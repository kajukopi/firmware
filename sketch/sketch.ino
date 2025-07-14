#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoOTA.h>
#include "tokens.h"
#include "webpage.h"

ESP8266WebServer server(80);
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

const int relayPin = D5;
const int servoPin = D6;

void notifyTelegram(const String& msg) {
  bot.sendMessage(CHAT_ID, msg, "");
}

void setup() {
  Serial.begin(115200);
  lcd.init(); lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("ESP8266 Starting");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 1); lcd.print("Connecting...");
    Serial.print(".");
  }

  lcd.clear(); lcd.setCursor(0, 0); lcd.print("WiFi Connected");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());

  client.setInsecure();
  servo.attach(servoPin);
  pinMode(relayPin, OUTPUT);

  server.on("/", []() { server.send_P(200, "text/html", MAIN_page); });

  server.on("/relay/on", []() {
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(0, 0); lcd.print("Relay: ON         ");
    notifyTelegram("Relay turned ON 🔥");
    server.send(200, "text/plain", "Relay ON");
  });

  server.on("/relay/off", []() {
    digitalWrite(relayPin, LOW);
    lcd.setCursor(0, 0); lcd.print("Relay: OFF        ");
    notifyTelegram("Relay turned OFF ❄️");
    server.send(200, "text/plain", "Relay OFF");
  });

  server.on("/servo/open", []() {
    servo.write(90);
    lcd.setCursor(0, 1); lcd.print("Servo: OPEN       ");
    notifyTelegram("Servo OPENED 🚪");
    server.send(200, "text/plain", "Servo OPEN");
  });

  server.on("/servo/close", []() {
    servo.write(0);
    lcd.setCursor(0, 1); lcd.print("Servo: CLOSED     ");
    notifyTelegram("Servo CLOSED 🚪");
    server.send(200, "text/plain", "Servo CLOSED");
  });

  server.begin();
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}
