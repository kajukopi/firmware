const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Smart Home Controller</title>
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsenui.min.css">
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsen-css-components.min.css">
  <style>
    body { background:#f3f3f3; }
    .center { text-align:center; margin:10px; }
    .servo-btn { width:30%; margin:2%; }
    .toggle-btn { width:40%; margin:2%; }
  </style>
</head>
<body>
  <ons-page>
    <ons-toolbar>
      <div class="center">Smart Home</div>
    </ons-toolbar>
    <ons-splitter>
      <ons-splitter-side id="menu" side="left" width="220px" collapse swipeable>
        <ons-page>
          <ons-list>
            <ons-list-item onclick="fn.load('home.html')">Home</ons-list-item>
            <ons-list-item onclick="fn.load('ota.html')">OTA Update</ons-list-item>
          </ons-list>
        </ons-page>
      </ons-splitter-side>
      <ons-splitter-content id="content">
        <div class="center" style="margin-top:16px;">
          <h2>Status</h2>
          <div id="lcd">LCD: Loading...</div>
          <h3>Servo Control</h3>
          <ons-row>
            <ons-col><ons-button class="servo-btn" onclick="servo(0)">0°</ons-button></ons-col>
            <ons-col><ons-button class="servo-btn" onclick="servo(90)">90°</ons-button></ons-col>
            <ons-col><ons-button class="servo-btn" onclick="servo(180)">180°</ons-button></ons-col>
          </ons-row>
          <h3>Relay & LED</h3>
          <ons-row>
            <ons-col><ons-switch id="relay-toggle" onchange="toggle('relay')"></ons-switch> Relay</ons-col>
            <ons-col><ons-switch id="led-toggle" onchange="toggle('led')"></ons-switch> LED</ons-col>
          </ons-row>
          <h3>OTA Update</h3>
          <input type="file" id="ota-file"/>
          <ons-button id="upload-btn" onclick="upload()">Upload</ons-button>
        </div>
      </ons-splitter-content>
    </ons-splitter>
  </ons-page>
  <script src="https://unpkg.com/onsenui/js/onsenui.min.js"></script>
  <script>
    // JS: Servo, Relay, LED, LCD update, OTA Upload
    function servo(deg) { fetch('/servo?angle='+deg); }
    function toggle(dev) { fetch('/toggle?device='+dev); }
    function upload() {
      var file = document.getElementById('ota-file').files[0];
      var form = new FormData();
      form.append("update", file);
      fetch('/update', {method:'POST',body:form});
    }
    setInterval(()=>fetch('/lcd').then(r=>r.text()).then(t=>document.getElementById('lcd').innerText=t),2000);
  </script>
</body>
</html>
)=====";