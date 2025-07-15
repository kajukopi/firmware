#ifndef WEBPAGE_H
#define WEBPAGE_H

const char WEB_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP8266 Panel</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsenui.min.css">
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsen-css-components.min.css">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css"/>
  <style>
    .log-block {
      background: #f8f9fa;
      padding: 1rem;
      border-radius: 10px;
      font-size: 0.85rem;
      max-height: 250px;
      overflow-y: auto;
      font-family: monospace;
      margin-top: 1em;
    }
    .center {
      text-align: center;
    }
    .spaced {
      margin-top: 1em;
      margin-bottom: 1em;
    }
  </style>
</head>
<body>
  <ons-navigator id="appNavigator" page="main.html"></ons-navigator>

  <template id="main.html">
    <ons-splitter>
      <ons-splitter-side id="menu" side="left" width="220px" collapse swipeable>
        <ons-page>
          <div class="center spaced"><i class="fas fa-microchip"></i> ESP8266</div>
          <ons-list>
            <ons-list-item tappable onclick="fn.loadPage('home')">
              <i class="fas fa-home"></i>&nbsp; Home
            </ons-list-item>
            <ons-list-item tappable onclick="fn.loadPage('update')">
              <i class="fas fa-upload"></i>&nbsp; OTA Update
            </ons-list-item>
          </ons-list>
        </ons-page>
      </ons-splitter-side>

      <ons-splitter-content id="content" page="home.html"></ons-splitter-content>
    </ons-splitter>
  </template>

  <!-- Home Page -->
  <template id="home.html">
    <ons-page>
      <ons-toolbar>
        <div class="left">
          <ons-toolbar-button onclick="fn.openMenu()">
            <ons-icon icon="fa-bars"></ons-icon>
          </ons-toolbar-button>
        </div>
        <div class="center">ESP8266 Control Panel</div>
      </ons-toolbar>

      <section style="margin: 1em;">
        <ons-card>
          <div class="center">
            <h5><i class="fas fa-satellite-dish"></i> Device Info</h5>
            <p><i class="fas fa-wifi"></i> IP Address: <span id="ip">-</span></p>
            <p><i class="fas fa-signal"></i> Signal Strength: <span id="signal">-</span></p>
          </div>
        </ons-card>

        <ons-card>
          <div class="center">
            <h5><i class="fas fa-terminal"></i> ESP Log</h5>
            <div id="espLog" class="log-block"></div>
          </div>
        </ons-card>
      </section>
    </ons-page>
  </template>

  <!-- Update Page -->
  <template id="update.html">
    <ons-page>
      <ons-toolbar>
        <div class="left">
          <ons-toolbar-button onclick="fn.openMenu()">
            <ons-icon icon="fa-bars"></ons-icon>
          </ons-toolbar-button>
        </div>
        <div class="center">OTA Firmware Update</div>
      </ons-toolbar>
      <section style="margin: 1em;">
        <ons-card>
          <div class="center">
            <h5><i class="fas fa-upload"></i> OTA Firmware Update</h5>
            <form method="POST" action="/update" enctype="multipart/form-data" class="mt-4">
              <input type="file" name="firmware" class="form-control mb-3" required style="margin-bottom: 1em;"/>
              <ons-button type="submit" modifier="large--cta" style="background: #4caf50; color: white;">Upload</ons-button>
            </form>
          </div>
        </ons-card>
      </section>
    </ons-page>
  </template>

  <script src="https://unpkg.com/onsenui/js/onsenui.min.js"></script>
  <script>
    window.fn = {};
    fn.openMenu = function() {
      document.getElementById('menu').open();
    };
    fn.loadPage = function(page) {
      document.getElementById('content').load(page + '.html');
      document.getElementById('menu').close();
    };

    // Update status info
    async function updateStatus() {
      try {
        const res = await fetch("/status");
        const data = await res.json();
        document.getElementById('ip').textContent = data.ip;
        document.getElementById('signal').textContent = data.signal;
      } catch (err) {
        // silent
      }
    }

    // Update ESP log
    async function updateESPLog() {
      try {
        const res = await fetch("/log");
        const data = await res.text();
        const logEl = document.getElementById("espLog");
        if (logEl) {
          logEl.textContent = data;
          logEl.scrollTop = logEl.scrollHeight;
        }
      } catch (err) {
        // silent
      }
    }

    // Init intervals when home page is shown
    let statusInt = null, logInt = null;

    document.addEventListener('init', function(event) {
      if (event.target.matches('#content')) {
        // do nothing
      }
      if (event.target.id === undefined) return;
      if (event.target.id === "home.html") {
        updateStatus(); updateESPLog();
        statusInt = setInterval(updateStatus, 3000);
        logInt = setInterval(updateESPLog, 3000);
      } else {
        if (statusInt) clearInterval(statusInt);
        if (logInt) clearInterval(logInt);
      }
    });
  </script>
</body>
</html>
)rawliteral";

#endif