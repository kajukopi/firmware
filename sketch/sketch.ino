#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Updater.h>

const char* ssid = "NAMA_WIFI_KAMU";
const char* password = "PASSWORD_WIFI_KAMU";

LiquidCrystal_I2C lcd(0x27, 16, 2); // Ganti 0x3F jika perlu
ESP8266WebServer server(80);

// HTML OTA Page with Bootstrap
const char* upload_html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>OTA Upload</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">
</head>
<body class="bg-dark text-white">
  <div class="container mt-5">
    <div class="card bg-secondary">
      <div class="card-header">
        <h3 class="text-center">ESP8266 OTA Update</h3>
      </div>
      <div class="card-body">
        <form method="POST" action="/update" enctype="multipart/form-data">
          <div class="mb-3">
            <label for="firmware" class="form-label">Pilih file firmware (.bin):</label>
            <input type="file" class="form-control" name="firmware">
          </div>
          <button type="submit" class="btn btn-warning w-100">Upload</button>
        </form>
      </div>
    </div>
    <p class="mt-3 text-center"><small>Powered by ESP8266</small></p>
  </div>
</body>
</html>
)rawliteral";

// Fungsi menampilkan 2 baris teks ke LCD
void lcdPrint(const String& line1, const String& line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {
  lcd.begin();
  lcd.backlight();

  lcdPrint("WiFi Connect", ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcdPrint("Connecting...", ".");
  }

  IPAddress ip = WiFi.localIP();
  lcdPrint("WiFi Terhubung", ip.toString());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", upload_html);
    lcdPrint("Akses Web", "/");
  });

  server.on("/update", HTTP_POST, []() {
    String msg = Update.hasError() ? "GAGAL Update" : "SUKSES Update";
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", msg + ", Restart...");
    lcdPrint(msg, "Restarting...");
    delay(1000);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      lcdPrint("Mulai Upload:", upload.filename.substring(0, 16));
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
        lcdPrint("Update Error!", "Begin failed");
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        lcdPrint("Write Error!", "");
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        lcdPrint("Update Sukses", String(upload.totalSize) + " byte");
      } else {
        lcdPrint("Update Gagal", "end() failed");
      }
    }
  });

  server.begin();
  lcdPrint("OTA Aktif", "Buka Web");
}

void loop() {
  server.handleClient();
}