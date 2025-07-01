#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Servo.h>
#include <WiFiUdp.h>
#include <Updater.h>

// Include HTML pages
#include "pages/page_index.h"
#include "pages/page_update.h"
#include "pages/page_reboot.h"

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);

// PIN setup
const int ledPin = 2;
const int servoPin = D5;
const int irPin = D6;  // IR Sensor OUT to D6 (GPIO12)

Servo myServo;

int currentBrightness = 100;
int currentServoAngle = 90;
bool objectDetected = false;

void handleRoot() {
  String html(PAGE_INDEX);
  html.replace("%BRIGHTNESS%", String(currentBrightness));
  html.replace("%SERVO%", String(currentServoAngle));
  server.send(200, "text/html", html);
}

void handleBrightness() {
  if (server.hasArg("value")) {
    currentBrightness = constrain(server.arg("value").toInt(), 0, 100);
    int pwm = map(100 - currentBrightness, 0, 100, 0, 1023);
    analogWrite(ledPin, pwm);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}

void handleServo() {
  if (server.hasArg("angle")) {
    currentServoAngle = constrain(server.arg("angle").toInt(), 0, 180);
    myServo.write(currentServoAngle);
    server.send(200, "text/plain", "Servo set");
  } else {
    server.send(400, "text/plain", "Missing angle");
  }
}

void handleIRSensor() {
  int state = digitalRead(irPin);
  objectDetected = (state == LOW);  // LOW = object detected
  String json = String("{\"objectDetected\":") + (objectDetected ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void handleOTAUploadForm() {
  server.send(200, "text/html", PAGE_UPDATE);
}

void handleRebootPage() {
  server.send(200, "text/html", PAGE_REBOOT);
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(ledPin, OUTPUT);
  pinMode(irPin, INPUT);
  analogWriteRange(1023);

  analogWrite(ledPin, map(100 - currentBrightness, 0, 100, 0, 1023));
  myServo.attach(servoPin, 600, 2400);
  myServo.write(currentServoAngle);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led/brightness", handleBrightness);
  server.on("/servo", handleServo);
  server.on("/sensor/ir", handleIRSensor);

  server.on("/update", HTTP_GET, handleOTAUploadForm);
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      WiFiUDP::stopAll();
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u bytes\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  server.on("/reboot", handleRebootPage);

  server.begin();
  ArduinoOTA.setHostname("NodeMCU-OTA");
  ArduinoOTA.begin();
  Serial.println("Server and OTA ready.");
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}