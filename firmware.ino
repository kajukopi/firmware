#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Servo.h>
#include <EEPROM.h> // ✅ EEPROM for saving settings

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const int ledPin = 2;      // Onboard LED (active LOW)
const int servoPin = D5;   // GPIO14

Servo myServo;

#define EEPROM_SIZE 4
#define ADDR_BRIGHTNESS 0
#define ADDR_SERVO 1

int currentBrightness = 100;      // default brightness
int currentServoAngle = 90;       // default angle

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>NodeMCU Control</title>
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
          font-size: 1.1rem;
          margin-top: 1rem;
        }
      </style>
    </head>
    <body>
      <h1>NodeMCU LED & Servo</h1>

      <div class="slider-label">LED Brightness: <span id="brightnessVal">%BRIGHTNESS%</span>%</div>
      <input type="range" id="brightness" min="0" max="100" value="%BRIGHTNESS%" oninput="setBrightness(this.value)">

      <div class="slider-label">Servo Angle: <span id="servoVal">%SERVO%</span>°</div>
      <input type="range" id="servo" min="0" max="180" value="%SERVO%" oninput="setServo(this.value)">

      <p><a href="/update"><button>OTA Firmware Upload</button></a></p>

      <script>
        function setBrightness(val) {
          document.getElementById("brightnessVal").innerText = val;
          fetch(`/led/brightness?value=${val}`)
            .then(res => console.log("LED brightness set:", val));
        }
        function setServo(val) {
          document.getElementById("servoVal").innerText = val;
          fetch(`/servo?angle=${val}`)
            .then(res => console.log("Servo angle set:", val));
        }
      </script>
    </body>
    </html>
  )rawliteral";

  html.replace("%BRIGHTNESS%", String(currentBrightness));
  html.replace("%SERVO%", String(currentServoAngle));
  server.send(200, "text/html", html);
}

void handleBrightness() {
  if (server.hasArg("value")) {
    currentBrightness = server.arg("value").toInt();
    currentBrightness = constrain(currentBrightness, 0, 100);
    int pwm = map(100 - currentBrightness, 0, 100, 0, 1023); // active LOW
    analogWrite(ledPin, pwm);

    // Save to EEPROM
    EEPROM.write(ADDR_BRIGHTNESS, currentBrightness);
    EEPROM.commit();

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}

void handleServo() {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    angle = constrain(angle, 0, 180);
    myServo.write(angle);
    currentServoAngle = angle;

    // Save to EEPROM
    EEPROM.write(ADDR_SERVO, currentServoAngle);
    EEPROM.commit();

    server.send(200, "text/plain", "Servo set");
  } else {
    server.send(400, "text/plain", "Missing angle");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(ledPin, OUTPUT);
  analogWriteRange(1023);

  EEPROM.begin(EEPROM_SIZE); // Init EEPROM

  // Load from EEPROM
  currentBrightness = EEPROM.read(ADDR_BRIGHTNESS);
  currentBrightness = constrain(currentBrightness, 0, 100);
  currentServoAngle = EEPROM.read(ADDR_SERVO);
  currentServoAngle = constrain(currentServoAngle, 0, 180);

  analogWrite(ledPin, map(100 - currentBrightness, 0, 100, 0, 1023));
  myServo.attach(servoPin, 600, 2400);
  myServo.write(currentServoAngle);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led/brightness", handleBrightness);
  server.on("/servo", handleServo);
  httpUpdater.setup(&server);

  server.begin();
  Serial.println("HTTP server started");

  ArduinoOTA.setHostname("NodeMCU-OTA");
  ArduinoOTA.begin();
  Serial.println("OTA Ready");

  Serial.print("Initial Brightness: ");
  Serial.println(currentBrightness);
  Serial.print("Initial Servo Angle: ");
  Serial.println(currentServoAngle);
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}