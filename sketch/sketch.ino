#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "tokens.h"
#include "webpage.h"

LiquidCrystal_I2C lcd(#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <ArduinoOTA.h>
#include "tokens.h"
#include "webpage.h"

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_SDA D2
#define LCD_SCL D1

#define SERVO_PIN D5
#define RELAY_PIN D6
#define LED_PIN LED_BUILTIN

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Servo myServo;
ESP8266WebServer server(80);

bool relayState = false;
bool ledState = false;
int servoAngle = 0;

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.print("RSSI: ");
  lcd.print(WiFi.RSSI());
}

void handleRoot() {
  server.send_P(200, "text/html", MAIN_page);
}

void handleServo() {
  if (server.hasArg("angle")) {
    servoAngle = server.arg("angle").toInt();
    myServo.write(servoAngle);
  }
  server.send(200, "text/plain", String("Servo: ") + servoAngle + " deg");
}

void handleToggle() {
  String device = server.arg("device");
  if (device == "relay") {
    relayState = !relayState;
    digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
    server.send(200, "text/plain", relayState ? "Relay ON" : "Relay OFF");
  } else if (device == "led") {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? LOW : HIGH); // LOW = ON for BUILTIN_LED
    server.send(200, "text/plain", ledState ? "LED ON" : "LED OFF");
  } else {
    server.send(400, "text/plain", "Bad device");
  }
}

void handleLCD() {
  String lcdText = "IP: " + WiFi.localIP().toString() + " | RSSI: " + String(WiFi.RSSI());
  server.send(200, "text/plain", lcdText);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("Connecting WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print(".");
  }

  lcd.clear(); lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/servo", handleServo);
  server.on("/toggle", handleToggle);
  server.on("/lcd", handleLCD);

  // OTA via Web
  server.on("/update", HTTP_POST, []() {
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      WiFiUDP::stopAll();
      Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
    yield();
  });

  server.onNotFound(handleNotFound);
  server.begin();

  ArduinoOTA.onStart([]() { lcd.clear(); lcd.print("OTA Update..."); });
  ArduinoOTA.onEnd([]() { lcd.clear(); lcd.print("OTA Done!"); });
  ArduinoOTA.begin();
  MDNS.begin("smarthome8266");
}

unsigned long lcdTimer = 0;
void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  if (millis() - lcdTimer > 2000) {
    updateLCD();
    lcdTimer = millis();
  }
}, 16, 2);
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