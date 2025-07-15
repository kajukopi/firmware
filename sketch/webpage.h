#ifndef WEBPAGE_H
#define WEBPAGE_H

const char WEB_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP8266 Panel</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css" rel="stylesheet"/>
  <link href="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/6.4.1/mdb.min.css" rel="stylesheet"/>
  <style>
    body { margin: 0; font-family: Arial, sans-serif; }
    .sidebar {
      height: 100%;
      width: 0;
      position: fixed;
      z-index: 1000;
      top: 0; left: 0;
      background-color: #0d6efd;
      overflow-x: hidden;
      transition: 0.3s;
      padding-top: 2rem;
      color: white;
    }
    .sidebar a {
      display: block;
      color: white;
      padding: 1rem 1.5rem;
      text-decoration: none;
    }
    .sidebar a:hover { background-color: #0b5ed7; }
    .sidebar .closebtn {
      position: absolute;
      top: 0.5rem;
      right: 1rem;
      font-size: 30px;
    }
    .main-content { margin-left: 0; padding: 0.5rem; transition: margin-left 0.3s; }
    .card { margin-bottom: 1rem; }
    .navbar { z-index: 1100; }
    .log-block {
      background: #f8f9fa;
      padding: 1rem;
      border-radius: 10px;
      font-size: 0.85rem;
      max-height: 250px;
      overflow-y: auto;
      font-family: monospace;
    }
  </style>
</head>
<body>
  <!-- Sidebar -->
  <div id="mySidebar" class="sidebar mt-5">
    <a href="javascript:void(0)" class="closebtn" onclick="closeSidebar()">&times;</a>
    <h5 class="text-center mt-3"><i class="fas fa-microchip"></i> ESP8266</h5>
    <a href="#" onclick="showPage('home')"><i class="fas fa-home"></i> Home</a>
    <a href="#" onclick="showPage('update')"><i class="fas fa-upload"></i> OTA Update</a>
  </div>

  <!-- Navbar -->
  <nav class="navbar navbar-expand-lg navbar-dark bg-primary">
    <div class="container-fluid">
      <button class="btn btn-light btn-sm" onclick="openSidebar()">
        <i class="fas fa-bars text-primary"></i>
      </button>
      <span class="navbar-brand ms-3">ESP8266 Control Panel</span>
    </div>
  </nav>

  <!-- Main Content -->
  <div class="main-content">
    <div class="container">
      <!-- Home Page -->
      <div id="home-page">
        <div class="card shadow-2-strong text-center">
          <div class="card-body">
            <h5 class="card-title"><i class="fas fa-satellite-dish"></i> Device Info</h5>
            <p><i class="fas fa-wifi"></i> IP Address: <span id="ip">-</span></p>
            <p><i class="fas fa-signal"></i> Signal Strength: <span id="signal">-</span></p>
          </div>
        </div>

        <div class="card shadow-2-strong text-center">
          <div class="card-body">
            <h5 class="card-title"><i class="fas fa-terminal"></i> ESP Log</h5>
            <div id="espLog" class="log-block"></div>
          </div>
        </div>
      </div>

      <!-- Update Page -->
      <div id="update-page" style="display:none">
        <div class="card shadow-2-strong text-center">
          <div class="card-body">
            <h5 class="card-title"><i class="fas fa-upload"></i> OTA Firmware Update</h5>
            <form method="POST" action="/update" enctype="multipart/form-data" class="mt-4">
              <input type="file" name="firmware" class="form-control mb-3" required>
              <input type="submit" value="Upload" class="btn btn-success">
            </form>
          </div>
        </div>
      </div>
    </div>
  </div>

<script>
  // Sidebar toggle
  function openSidebar() {
    document.getElementById("mySidebar").style.width = "220px";
  }

  function closeSidebar() {
    document.getElementById("mySidebar").style.width = "0";
  }

  function showPage(page) {
    document.getElementById("home-page").style.display = (page === "home") ? "block" : "none";
    document.getElementById("update-page").style.display = (page === "update") ? "block" : "none";
    closeSidebar();
  }

  // Update status info
  async function updateStatus() {
    try {
      const res = await fetch("/status");
      const data = await res.json();
      document.getElementById('ip').textContent = data.ip;
      document.getElementById('signal').textContent = data.signal;
    } catch (err) {
      console.error("Failed to fetch status:", err);
    }
  }

  // Update ESP log
  async function updateESPLog() {
    try {
      const res = await fetch("/log");
      const data = await res.text();
      const logEl = document.getElementById("espLog");
      logEl.textContent = data;
      logEl.scrollTop = logEl.scrollHeight;
    } catch (err) {
      console.error("Failed to fetch ESP log:", err);
    }
  }

  // Init
  setInterval(updateStatus, 3000);
  setInterval(updateESPLog, 3000);
  updateStatus();
  updateESPLog();
  showPage('home');
</script>
</body>
</html>
)rawliteral";

#endif