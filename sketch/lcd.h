#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;
extern unsigned long lcdRestoreTime;
extern unsigned long lastStatusDisplay;

void lcdInit();
void lcdShowStartup();
void lcdShowConnecting();
void lcdShowSearching();
void lcdShowIp(const String& ip);
void lcdShowTempMessage(const String& line1, const String& line2 = "");
void lcdShowSignalStrength(const String& rssi);

#endif