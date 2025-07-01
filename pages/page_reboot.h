const char PAGE_REBOOT[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Rebooting</title>
  <style>
    body { background: #f7f7f7; font-family: Arial; text-align: center; padding: 2em; }
    h1 { color: #FF5722; }
    p { font-size: 1.2rem; color: #333; }
    .loader {
      margin: 40px auto; border: 6px solid #f3f3f3;
      border-top: 6px solid #FF5722;
      border-radius: 50%; width: 50px; height: 50px;
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