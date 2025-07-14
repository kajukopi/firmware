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
<button onclick="fetch('/servo/close')">Servo CLOSE</button>
</body></html>
)rawliteral";

#endif
