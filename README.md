# 📦 ESP8266 Smart IoT Project: LCD, Relay, Servo, Web Control, Telegram Notification & OTA Update

Proyek ini menggabungkan berbagai komponen ESP8266 yang mudah didapat dan cocok untuk pemula. Perangkat ini dikendalikan melalui antarmuka web dan terhubung ke Telegram untuk mengirimkan notifikasi. Selain itu, proyek ini juga mendukung Over The Air (OTA) update otomatis dari GitHub dan halaman manual upload firmware melalui web UI.

---

## 🧰 Hardware yang Digunakan

- LOLIN ESP8266 NodeMCU V1  
- LCD 16x2 I2C  
- Micro Servo 9g  
- Single Relay HW-482 3 pin (S, +, -)

---

## 📡 Konfigurasi

**WiFi**
- SSID: `karimroy`
- Password: `09871234`

**Telegram Bot**
- Token: `7826449058:AAHZgPVpCdmwHsmCO6D9wIsnpZj3fOyjXWM`
- Chat ID: `7891041281`

---

## 📁 Struktur Proyek

```
project-root/
├── .github/
│   └── workflows/
│       └── compile.yml
├── sketch/
│   ├── sketch.ino
│   ├── tokens.h
│   └── webpage.h
└── README.md
```

---

## 🔑 File: `tokens.h`

```cpp
#ifndef TOKENS_H
#define TOKENS_H

#define WIFI_SSID     "karimroy"
#define WIFI_PASSWORD "09871234"

#define BOT_TOKEN     "7826449058:AAHZgPVpCdmwHsmCO6D9wIsnpZj3fOyjXWM"
#define CHAT_ID       "7891041281"

#endif
```

---

## 🌐 File: `webpage.h`

```cpp
#ifndef WEBPAGE_H
#define WEBPAGE_H

const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP8266 Control</title></head>
<body>
<h2>ESP8266 Control Panel</h2>
<button onclick="fetch('/relay/on')">Relay ON</button>
<button onclick="fetch('/relay/off')">Relay OFF</button><br><br>
<button onclick="fetch('/servo/open')">Servo OPEN</button>
<button onclick="fetch('/servo/close')">Servo CLOSE</button><br><br>
<form method="POST" action="/update" enctype="multipart/form-data">
  <input type="file" name="update">
  <input type="submit" value="Upload Firmware">
</form>
<br>
<button onclick="fetch('/ota')">OTA Update from GitHub</button>
</body></html>
)rawliteral";

#endif
```

---

## 🔧 File: `sketch.ino`

```cpp
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

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

const int relayPin = D5;
const int servoPin = D6;

void notifyTelegram(const String& msg) {
  bot.sendMessage(CHAT_ID, msg, "");
}

void otaUpdateFromGitHub() {
  WiFiClient client;
  t_httpUpdate_return ret = ESPhttpUpdate.update(
    client,
    "https://github.com/[USERNAME]/[REPO]/releases/latest/download/latest-release.bin"
  );

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("Update failed.");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No updates.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update successful.");
      break;
  }
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

  server.on("/ota", []() {
    server.send(200, "text/plain", "Starting OTA update...");
    otaUpdateFromGitHub();
  });

  httpUpdater.setup(&server, "/update");
  server.begin();
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}
```

---

## 🚀 File: `.github/workflows/compile.yml`

```yaml
name: 🚀 Build & Release ESP8266 Firmware

on:
  push:
    paths:
      - 'sketch/**'
  workflow_dispatch:  # Bisa dijalankan manual dari GitHub UI

jobs:
  build:
    name: 🔧 Compile & Release Firmware
    runs-on: ubuntu-latest

    steps:
      - name: 📥 Checkout Repository
        uses: actions/checkout@v4

      - name: 🛠️ Install Arduino CLI
        uses: arduino/setup-arduino-cli@v1
        with:
          version: latest

      - name: 📦 Install ESP8266 Board Package
        run: |
          arduino-cli core update-index
          arduino-cli core install esp8266:esp8266

      - name: 🧪 Compile sketch.ino
        run: |
          arduino-cli compile \
            --fqbn esp8266:esp8266:nodemcuv2 \
            --output-dir ./build \
            ./sketch

      - name: 📝 Rename binary output
        run: |
          mv ./build/sketch.ino.bin ./build/latest-release.bin

      - name: 🚀 Create GitHub Release & Upload .bin
        uses: softprops/action-gh-release@v2
        with:
          tag_name: latest
          name: 🌟 Firmware Latest Release
          body: |
            🔄 Update otomatis hasil kompilasi dari folder `sketch/`
            📅 Waktu: ${{ github.event.head_commit.timestamp }}
            ✏️ Commit: ${{ github.event.head_commit.message }}
          files: |
            ./build/latest-release.bin
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

      - name: 📦 Upload Artifact (Backup Binary)
        uses: actions/upload-artifact@v4
        with:
          name: latest-release
          path: ./build/latest-release.bin
```

---

## 📥 OTA Update Otomatis & Manual

- 🔄 **Otomatis dari GitHub Release**:
  - Tekan tombol **"OTA Update from GitHub"** di web UI.
  - File akan diambil dari:  
    `https://github.com/[USERNAME]/[REPO]/releases/latest/download/latest-release.bin`

- 💾 **Upload Manual**:
  - Buka `http://device_ip/update`
  - Pilih file `.bin` lalu klik Upload

---

## ✅ Badge Status Build

![Compile Firmware](https://github.com/kajukopi/firmware/actions/workflows/compile.yml/badge.svg)

---

## 🧠 Catatan

- LCD Address I2C menggunakan `0x27`, pastikan sesuai dengan modulmu.
- OTA hanya berfungsi jika perangkat terhubung ke jaringan WiFi yang sama.
- Ganti `[USERNAME]` dan `[REPO]` dengan akun dan nama repositori kamu di GitHub.
- File `latest-release.bin` akan selalu otomatis diperbarui oleh GitHub Actions setelah push ke folder `sketch/`.

---