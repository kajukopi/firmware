#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Servo.h>
#include <WiFiUdp.h>
#include <Updater.h>

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);

const int ledPin = 2;
const int servoPin = D5;

Servo myServo;

int currentBrightness = 100;  // default runtime
int currentServoAngle = 90;

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
          cursor: pointer;
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
      <p><a href="/reboot"><button style="background:#FF5722;">Reboot Device</button></a></p>

      <script>
        function setBrightness(val) {
          document.getElementById("brightnessVal").innerText = val;
          fetch(`/led/brightness?value=${val}`);
        }
        function setServo(val) {
          document.getElementById("servoVal").innerText = val;
          fetch(`/servo?angle=${val}`);
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

void handleOTAUploadForm() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>OTA Firmware Upload</title>
      <style>
        body {
          background: #f7f7f7;
          font-family: Arial, sans-serif;
          text-align: center;
          padding: 2em;
        }
        h1 {
          color: #4CAF50;
        }
        form {
          background: white;
          display: inline-block;
          padding: 2em;
          border-radius: 10px;
          box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        input[type="file"] {
          padding: 1em;
          margin-bottom: 1em;
          width: 100%;
        }
        input[type="submit"] {
          background: #4CAF50;
          border: none;
          padding: 10px 20px;
          color: white;
          border-radius: 6px;
          cursor: pointer;
        }
        a {
          display: block;
          margin-top: 20px;
          color: #4CAF50;
          text-decoration: none;
        }
      </style>
    </head>
    <body>
      <h1>Upload Firmware (OTA)</h1>
      <form method="POST" action="/update" enctype="multipart/form-data">
        <input type="file" name="firmware" required>
        <br>
        <input type="submit" value="Upload">
      </form>
      <a href="/">← Back to Control Panel</a>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleRebootPage() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Rebooting</title>
      <style>
        body {
          background: #f7f7f7;
          font-family: Arial, sans-serif;
          text-align: center;
          padding: 2em;
        }
        h1 { color: #FF5722; }
        p { font-size: 1.2rem; color: #333; }
        .loader {
          margin: 40px auto;
          border: 6px solid #f3f3f3;
          border-top: 6px solid #FF5722;
          border-radius: 50%;
          width: 50px;
          height: 50px;
          animation: spin 1s linear infinite;
        }
        @keyframes spin {
          0% { transform: rotate(0deg); }
          100% { transform: rotate(360deg); }
        }
      </style>
      <script>
        setTimeout(() => {
          location.href = "/";
        }, 8000);
      </script>
    </head>
    <body>
      <h1>Rebooting...</h1>
      <div class="loader"></div>
      <p>Device is restarting. You will be redirected automatically.</p>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(ledPin, OUTPUT);
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

  // Web routes
  server.on("/", handleRoot);
  server.on("/led/brightness", handleBrightness);
  server.on("/servo", handleServo);
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