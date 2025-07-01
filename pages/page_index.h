const char PAGE_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>NodeMCU Control</title>
  <style>
    body { font-family: Arial; background: #f7f7f7; text-align: center; padding: 2em; }
    h1 { color: #4CAF50; }
    button {
      padding: 10px 20px; margin: 10px; border: none;
      background: #4CAF50; color: white; border-radius: 6px; cursor: pointer;
    }
    input[type="range"] { width: 80%; margin: 20px auto; }
    .slider-label { font-size: 1.1rem; margin-top: 1rem; }
  </style>
</head>
<body>
  <h1>NodeMCU LED & Servo</h1>

  <div class="slider-label">LED Brightness: <span id="brightnessVal">%BRIGHTNESS%</span>%</div>
  <input type="range" id="brightness" min="0" max="100" value="%BRIGHTNESS%" oninput="setBrightness(this.value)">

  <div class="slider-label">Servo Angle: <span id="servoVal">%SERVO%</span>°</div>
  <input type="range" id="servo" min="0" max="180" value="%SERVO%" oninput="setServo(this.value)">

  <div class="slider-label">IR Sensor: <span id="irState">Loading...</span></div>

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
    setInterval(() => {
      fetch('/sensor/ir')
        .then(res => res.json())
        .then(data => {
          document.getElementById("irState").innerText = data.objectDetected ? "OBSTACLE" : "CLEAR";
        });
    }, 1000);
  </script>
</body>
</html>
)rawliteral";