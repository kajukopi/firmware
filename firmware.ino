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
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>NodeMCU LED Control</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          background: #f7f7f7;
          color: #333;
          margin: 0;
          padding: 0;
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
          height: 100vh;
          text-align: center;
        }
        h1 {
          color: #4CAF50;
          margin-bottom: 10px;
        }
        .status {
          margin: 10px 0;
          font-size: 1.2rem;
        }
        button {
          display: inline-block;
          padding: 10px 20px;
          margin: 10px;
          font-size: 1rem;
          color: #fff;
          background-color: #4CAF50;
          border: none;
          border-radius: 8px;
          cursor: pointer;
          transition: background-color 0.3s;
        }
        button:hover {
          background-color: #45a049;
        }
        a {
          text-decoration: none;
        }
        .footer {
          margin-top: 20px;
          font-size: 0.9rem;
        }
      </style>
    </head>
    <body>
      <h1>NodeMCU LED Control</h1>
      <div class="status">LED is <b>%STATE%</b></div>
      <a href="/led/on"><button>Turn ON</button></a>
      <a href="/led/off"><button>Turn OFF</button></a>
      <div class="footer">
        <p><a href="/update"><button>OTA Firmware Upload</button></a></p>
      </div>
    </body>
    </html>
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

// Edit