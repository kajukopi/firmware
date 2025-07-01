const char PAGE_UPDATE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>OTA Firmware Upload</title>
  <style>
    body { background: #f7f7f7; font-family: Arial; text-align: center; padding: 2em; }
    h1 { color: #4CAF50; }
    form {
      background: white; display: inline-block;
      padding: 2em; border-radius: 10px;
      box-shadow: 0 0 10px rgba(0,0,0,0.1);
    }
    input[type="file"] {
      padding: 1em; margin-bottom: 1em; width: 100%;
    }
    input[type="submit"] {
      background: #4CAF50; border: none;
      padding: 10px 20px; color: white;
      border-radius: 6px; cursor: pointer;
    }
    a {
      display: block; margin-top: 20px;
      color: #4CAF50; text-decoration: none;
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