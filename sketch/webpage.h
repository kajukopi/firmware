#ifndef WEBPAGE_H
#define WEBPAGE_H

const char WEB_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <title>ESP OTA + Firebase</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/onsenui/css/onsenui.min.css" />
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/onsenui/css/onsen-css-components.min.css" />
    <script src="https://cdn.jsdelivr.net/npm/onsenui/js/onsenui.min.js"></script>
  </head>
  <body>
    <ons-page>
      <ons-toolbar>
        <div class="center">ESP8266 Control</div>
      </ons-toolbar>
      <div style="text-align: center; margin-top: 30px;">
        <form action="/post" method="POST">
          <ons-input name="message" placeholder="Message" float></ons-input><br><br>
          <ons-button type="submit">Send to Firebase</ons-button>
        </form>
      </div>
    </ons-page>
  </body>
</html>
)rawliteral";

#endif