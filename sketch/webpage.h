#ifndef WEBPAGE_H
#define WEBPAGE_H

const char WEB_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>ESP8266 Panel</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- Bootstrap core CSS -->
  <link href="https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/5.3.2/css/bootstrap.min.css" rel="stylesheet" />
  <!-- MDBootstrap core CSS -->
  <link href="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/6.4.2/mdb.min.css" rel="stylesheet" />
  <!-- Font Awesome -->
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.2/css/all.min.css"/>
  <style>
    .log-block {
      background: #f8f9fa;
      padding: 1rem;
      border-radius: 10px;
      font-size: 0.9rem;
      max-height: 250px;
      overflow-y: auto;
      font-family: monospace;
      margin-top: 1em;
    }
  </style>
</head>
<body class="bg-light">
  <div class="container py-4">
    <div class="row mb-4">
      <div class="col text-center">
        <h2><i class="fas fa-microchip"></i> ESP8266 Control Panel</h2>
      </div>
    </div>

    <!-- Tabs Navigation -->
    <ul class="nav nav-tabs mb-3" id="panelTabs" role="tablist">
      <li class="nav-item" role="presentation">
        <button class="nav-link active" id="device-tab" data-bs-toggle="tab" data-bs-target="#device" type="button" role="tab"><i class="fas fa-satellite-dish"></i> Device Info</button>
      </li>
      <li class="nav-item" role="presentation">
        <button class="nav-link" id="ota-tab" data-bs-toggle="tab" data-bs-target="#ota" type="button" role="tab"><i class="fas fa-upload"></i> OTA Update</button>
      </li>
      <li class="nav-item" role="presentation">
        <button class="nav-link" id="log-tab" data-bs-toggle="tab" data-bs-target="#log" type="button" role="tab"><i class="fas fa-terminal"></i> ESP Log</button>
      </li>
    </ul>

    <!-- Tabs Content -->
    <div class="tab-content" id="panelTabsContent">
      <!-- Device Info Tab -->
      <div class="tab-pane fade show active" id="device" role="tabpanel">
        <div class="card shadow mb-4">
          <div class="card-body text-center">
            <h5 class="card-title"><i class="fas fa-satellite-dish"></i> Device Info</h5>
            <p><i class="fas fa-wifi"></i> IP Address: <span id="ip">-</span></p>
            <p><i class="fas fa-signal"></i> Signal Strength: <span id="signal">-</span></p>
          </div>
        </div>
      </div>

      <!-- OTA Update Tab -->
      <div class="tab-pane fade" id="ota" role="tabpanel">
        <div class="card shadow mb-4">
          <div class="card-body text-center">
            <h5 class="card-title"><i class="fas fa-upload"></i> OTA Firmware Update</h5>
            <form method="POST" action="/update" enctype="multipart/form-data">
              <input type="file" name="firmware" class="form-control mb-3" required />
              <button type="submit" class="btn btn-success btn-block"><i class="fas fa-upload"></i> Upload</button>
            </form>
          </div>
        </div>
      </div>

      <!-- ESP Log Tab -->
      <div class="tab-pane fade" id="log" role="tabpanel">
        <div class="card shadow mb-4">
          <div class="card-body">
            <h5 class="card-title text-center"><i class="fas fa-terminal"></i> ESP Log</h5>
            <div id="espLog" class="log-block"></div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- Bootstrap JS (needed for tabs to work) -->
  <script src="https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/5.3.2/js/bootstrap.bundle.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/6.4.2/mdb.min.js"></script>
  <script>
    // Status updater
    async function updateStatus() {
      try {
        const res = await fetch("/status");
        const data = await res.json();
        document.getElementById('ip').textContent = data.ip;
        document.getElementById('signal').textContent = data.signal;
      } catch (err) {}
    }

    // Log updater
    async function updateESPLog() {
      try {
        const res = await fetch("/log");
        const data = await res.text();
        const logEl = document.getElementById("espLog");
        if (logEl) {
          logEl.textContent = data;
          logEl.scrollTop = logEl.scrollHeight;
        }
      } catch (err) {}
    }

    setInterval(updateStatus, 3000);
    setInterval(updateESPLog, 3000);
    updateStatus();
    updateESPLog();
  </script>
</body>
</html>
)rawliteral";

#endif