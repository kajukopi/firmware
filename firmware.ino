#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const int ledPin = 2; // Onboard LED (active LOW)

void handleRoot() {
  int currentPwm = analogRead(ledPin); // Note: not always accurate on ESP8266
  int brightness = map(1023 - currentPwm, 0, 1023, 0, 100); // Invert for active LOW

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
          text-align: center;
          padding: 2em;
        }
        h1 { color: #4CAF50; }
        button {
          padding: 10px 20px;
          margin: 10px;
          border: none;
          background: #4CAF50;
          color: white;
          border-radius: 6px;
        }
        input[type="range"] {
          width: 80%;
          margin: 20px auto;
        }
        .slider-label {
          font-size: 1.2rem;
        }
      </style>
    </head>
    <body>
      <h1>NodeMCU LED Control</h1>
      <div class="slider-label">Brightness: <span id="brightnessVal">%BRIGHTNESS%</span>%</div>
      <input type="range" id="brightness" min="0" max="100" value="%BRIGHTNESS%" oninput="setBrightness(this.value)">
      <p><a href="/update"><button>OTA Firmware Upload</button></a></p>

      <script>
        function setBrightness(val) {
          document.getElementById("brightnessVal").innerText = val;
          fetch(`/led/brightness?value=${val}`)
            .then(res => console.log("Brightness set to", val));
        }
      </script>
    </body>
    </html>
  )rawliteral";

  html.replace("%BRIGHTNESS%", String(brightness));
  server.send(200, "text/html", html);
}

void handleBrightness() {
  if (server.hasArg("value")) {
    int brightness = server.arg("value").toInt();
    brightness = constrain(brightness, 0, 100);
    int pwm = map(100 - brightness, 0, 100, 0, 1023); // Invert for active LOW
    analogWrite(ledPin, pwm);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(ledPin, OUTPUT);
  analogWriteRange(1023);
  analogWrite(ledPin, 1023); // LED OFF (active LOW)

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected at:");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led/brightness", handleBrightness);
  httpUpdater.setup(&server);

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