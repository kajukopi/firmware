#include "lcd.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lcdRestoreTime = 0;
unsigned long lastStatusDisplay = 0;

void lcdInit() {
  lcd.init();
  lcd.backlight();
}

void lcdShowStartup() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("ESP8266");
  lcd.setCursor(0, 1); lcd.print("Web Server");
  delay(2000);
}

void lcdShowConnecting() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Connecting WiFi");
}

void lcdShowSearching() {
  lcd.setCursor(0, 1); lcd.print("Searching...");
}

void lcdShowIp(const String& ip) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print(ip);
}

void lcdShowTempMessage(const String& line1, const String& line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  lcdRestoreTime = millis() + 3000;
}

void lcdShowSignalStrength(const String& rssi) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Signal Strength");
  lcd.setCursor(0, 1);
  lcd.print(rssi.substring(0, 16));
  lcdRestoreTime = millis() + 2000;
}