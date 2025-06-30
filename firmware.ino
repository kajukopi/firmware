#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

const char* ssid = "karimroy";
const char* password = "09871234";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const int ledPin = LED_BUILTIN;
bool ledState = false;

const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; background: #f9f9f9; text-align: center; padding: 20px; }
    button { font-size: 20px; padding: 10px 30px; margin: 10px; }
    input[type=file], input[type=submit] { margin-top: 15px; font-size: 16px; }
  </style>
</head>
<body>
  <h2>NodeMCU Control</h2>
  <button onclick="toggleLED()">Toggle LED</button><br><br>
  <form method="POST" action="/update" enctype="multipart/form-data">
    <input type="file" name="update"><br>
    <input type="submit" value="Update Firmware">
  </form>
  <script>
    function toggleLED() {
      fetch('/toggle')
        .then(res => res.text())
        .then(txt => alert('LED is ' + txt));
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);  // LED OFF

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", html);
  });

  server.on("/toggle", HTTP_GET, []() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? LOW : HIGH);
    server.send(200, "text/plain", ledState ? "ON" : "OFF");
  });

  httpUpdater.setup(&server);
  server.begin();
}

void loop() {
  server.handleClient();
}

// Edit Files 1