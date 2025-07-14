#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>
#include "tokens.h"
#include "webpage.h"

// Inisialisasi
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClientSecure secureClient;
UniversalTelegramBot bot(BOT_TOKEN, secureClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C LCD
Servo servo;

// Pin
const int relayPin = D5;
const int servoPin = D6;

// Fungsi Kirim ke Telegram
void notifyTelegram(const String& msg) {
  bot.sendMessage(CHAT_ID, msg, "");
}

// Fungsi OTA Update dari GitHub Release
void otaUpdateFromGitHub() {
  WiFiClient client;
  String url = "http://yourdomain.com/latest-release.bin"; // Ganti dengan file OTA langsung (gunakan domain HTTP)
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, url);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("OTA Update Failed: %s\n", ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No OTA Update available.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("OTA Update Success! Rebooting...");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init(); lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("ESP8266 Starting");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0, 1); lcd.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Tampilkan IP di LCD
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("IP:");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());

  // Telegram
  secureClient.setInsecure(); // Hapus validasi SSL

  // Setup pin
  pinMode(relayPin, OUTPUT);
  servo.attach(servoPin);

  // Web endpoint
  server.on("/", []() { server.send_P(200, "text/html", MAIN_page); });

  server.on("/relay/on", []() {
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(0, 0); lcd.print("Relay: ON         ");
    notifyTelegram("🔌 Relay turned ON");
    server.send(200, "text/plain", "Relay ON");
  });

  server.on("/relay/off", []() {
    digitalWrite(relayPin, LOW);
    lcd.setCursor(0, 0); lcd.print("Relay: OFF        ");
    notifyTelegram("⚡ Relay turned OFF");
    server.send(200, "text/plain", "Relay OFF");
  });

  server.on("/servo/open", []() {
    servo.write(90);
    lcd.setCursor(0, 1); lcd.print("Servo: OPEN       ");
    notifyTelegram("🚪 Servo OPENED");
    server.send(200, "text/plain", "Servo OPEN");
  });

  server.on("/servo/close", []() {
    servo.write(0);
    lcd.setCursor(0, 1); lcd.print("Servo: CLOSED     ");
    notifyTelegram("🔒 Servo CLOSED");
    server.send(200, "text/plain", "Servo CLOSED");
  });

  server.on("/ota", []() {
    server.send(200, "text/plain", "Starting OTA update...");
    otaUpdateFromGitHub();
  });

  // Manual OTA Upload via /update
  httpUpdater.setup(&server, "/update");

  server.begin();
  ArduinoOTA.begin();

  Serial.println("HTTP server started.");
  Serial.println("IP address: " + WiFi.localIP().toString());
}
  
void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}