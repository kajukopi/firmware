#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <UniversalTelegramBot.h>
#include "tokens.h"
#include "webpage.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); // Cek alamat I2C kalau perlu
Servo myservo;
ESP8266WebServer server(80);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

#define SERVO_PIN D5
#define RELAY_PIN D6
#define LED_PIN LED_BUILTIN

String scrollText = "";
int scrollIdx = 0;
unsigned long lastLcdUpdate = 0;
unsigned long lastBotTime = 0;

void setup() {
  Serial.begin(115200);
  lcd.init(); lcd.backlight();
  pinMode(RELAY_PIN, OUTPUT); digitalWrite(RELAY_PIN, LOW);
  pinMode(LED_PIN, OUTPUT); digitalWrite(LED_PIN, HIGH); // LED_BUILTIN aktif LOW
  myservo.attach(SERVO_PIN);
  lcd.setCursor(0,0); lcd.print("WiFi Connecting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); lcd.print(".");
  }
  IPAddress ip = WiFi.localIP();
  String info = "IP:" + ip.toString() + " SSID:" + String(WIFI_SSID);
  scrollText = info + " "; // animasi infinite

  lcd.clear(); lcd.print("IP:"); lcd.print(ip);

  // OTA
  ArduinoOTA.onStart([]() { lcd.clear(); lcd.print("OTA Update"); });
  ArduinoOTA.begin();

  // Telegram SSL
  secured_client.setInsecure();

  // Webserver
  server.on("/", []() {
    server.send_P(200, "text/html", MAIN_page);
  });
  server.on("/lcd", []() {
    server.send(200, "text/plain", scrollText);
  });
  server.on("/servo", []() {
    if(server.hasArg("pos")) myservo.write(server.arg("pos").toInt());
    server.send(200, "text/plain", "OK");
  });
  server.on("/relay", []() {
    if(server.hasArg("state")) digitalWrite(RELAY_PIN, server.arg("state").toInt()?HIGH:LOW);
    server.send(200, "text/plain", "OK");
  });
  server.on("/led", []() {
    if(server.hasArg("state")) digitalWrite(LED_PIN, server.arg("state").toInt()?LOW:HIGH);
    server.send(200, "text/plain", "OK");
  });
  server.on("/update", HTTP_POST, [](){
    server.send(200,"text/plain","OK"); ESP.restart();
  },[](){
    HTTPUpload& upload = server.upload();
    if(upload.status==UPLOAD_FILE_START) {ArduinoOTA.begin();}
    if(upload.status==UPLOAD_FILE_WRITE) {Update.write(upload.buf, upload.currentSize);}
    if(upload.status==UPLOAD_FILE_END) {Update.end(true);}
  });
  server.begin();

  // Kirim notifikasi Telegram saat boot
  bot.sendMessage(CHAT_ID, "ESP8266 started! IP: " + ip.toString(), "");
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  // LCD animasi moving text
  if(millis() - lastLcdUpdate > 400){
    lcd.clear();
    String line1 = scrollText.substring(scrollIdx, scrollIdx+16);
    lcd.setCursor(0,0); lcd.print(line1);
    scrollIdx++;
    if(scrollIdx > scrollText.length()-16) scrollIdx = 0;
    lastLcdUpdate = millis();
  }

  // Telegram polling
  if(millis() - lastBotTime > 5000){
    int n = bot.getUpdates(bot.last_message_received + 1);
    for(int i=0;i<n;i++){
      String text = bot.messages[i].text;
      if(text == "/status") {
        bot.sendMessage(CHAT_ID, "IP: "+WiFi.localIP().toString(), "");
      }
    }
    lastBotTime = millis();
  }
}