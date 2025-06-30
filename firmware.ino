#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const int ledPin = 2; // Onboard LED

void handleRoot() {
  String html = R"rawliteral(
    <h1>NodeMCU LED Control</h1>
    <p>LED is <b>%STATE%</b></p>
    <a href="/led/on"><button>Turn ON</button></a>
    <a href="/led/off"><button>Turn OFF</button></a>
    <p><a href="/update">OTA Firmware Upload</a></p>
  )rawliteral";

  html.replace("%STATE%", digitalRead(ledPin) == LOW ? "ON" : "OFF");
  server.send(200, "text/html", html);
}

void handleLedOn() {
  digitalWrite(ledPin, LOW); // LED ON (active LOW)
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLedOff() {
  digitalWrite(ledPin, HIGH); // LED OFF
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Start with LED OFF

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected at: ");
  Serial.println(WiFi.localIP());

  // Web server routes
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  // OTA firmware page
  httpUpdater.setup(&server); // /update

  server.begin();
  Serial.println("HTTP server started");

  ArduinoOTA.setHostname("NodeMCU-OTA");
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}