const char MAIN_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP8266 Control</title>
  <link href="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/7.4.0/mdb.min.css" rel="stylesheet"/>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/7.4.0/css/all.min.css"/>
  <meta name="viewport" content="width=device-width, initial-scale=1"/>
  <style>
    body {padding:0;margin:0;}
    #lcd {font-family:monospace;background:#222;color:#0f0;padding:10px;border-radius:5px;}
    .sidebar {position:fixed;left:0;top:0;width:220px;height:100vh;background:#212529;z-index:2000;display:none;}
    .overlay {position:fixed;top:0;left:0;width:100vw;height:100vh;background:rgba(0,0,0,.4);z-index:1999;display:none;}
    .btn{margin:5px;}
    ul{list-style:none;padding:0;}
    ul li a{display:block;padding:8px 15px;}
  </style>
</head>
<body>
<div class="overlay" id="overlay" onclick="closeSidebar()"></div>
<div class="sidebar" id="sidebar">
  <h4 class="text-white p-3">Menu</h4>
  <ul>
    <li><a href="#" class="text-white" onclick="page('home')"><i class="fa fa-home"></i> Home</a></li>
    <li><a href="#" class="text-white" onclick="page('ota')"><i class="fa fa-upload"></i> OTA Update</a></li>
  </ul>
</div>
<nav class="navbar navbar-dark bg-dark">
  <a class="navbar-brand" href="#"><i class="fa fa-microchip"></i> ESP8266 Control</a>
  <button class="btn btn-outline-light" onclick="openSidebar()"><i class="fa fa-bars"></i></button>
</nav>
<div class="container mt-4" id="main-content"></div>
<script>
function openSidebar(){document.getElementById('sidebar').style.display='block';document.getElementById('overlay').style.display='block';}
function closeSidebar(){document.getElementById('sidebar').style.display='none';document.getElementById('overlay').style.display='none';}
function page(p){
  if(p==='ota'){document.getElementById('main-content').innerHTML = `<h3>OTA Update</h3>
    <form method='POST' action='/update' enctype='multipart/form-data'>
      <input type='file' name='update' /><input type='submit' value='Update'/>
    </form>`;}
  else{loadHome();}
  closeSidebar();
}
function loadHome(){
  document.getElementById('main-content').innerHTML = `
    <div id="lcd"></div>
    <h5>Servo</h5>
    <button class="btn btn-primary" onclick="servo(0)">0°</button>
    <button class="btn btn-primary" onclick="servo(45)">45°</button>
    <button class="btn btn-primary" onclick="servo(90)">90°</button>
    <h5>Relay</h5>
    <button class="btn btn-success" onclick="relay(1)">ON</button>
    <button class="btn btn-danger" onclick="relay(0)">OFF</button>
    <h5>LED</h5>
    <button class="btn btn-success" onclick="led(1)">ON</button>
    <button class="btn btn-danger" onclick="led(0)">OFF</button>
  `;
  fetch('/lcd').then(r=>r.text()).then(t=>document.getElementById('lcd').textContent=t);
}
function servo(pos){fetch('/servo?pos='+pos);}
function relay(val){fetch('/relay?state='+val);}
function led(val){fetch('/led?state='+val);}
window.onload=loadHome;
</script>
</body>
</html>
)=====";