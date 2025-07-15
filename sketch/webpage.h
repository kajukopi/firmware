#ifndef WEBPAGE_H
#define WEBPAGE_H

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>OTA & LCD Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- Onsen UI Stable -->
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsenui.min.css">
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsen-css-components.min.css">
  <script src="https://unpkg.com/onsenui/js/onsenui.min.js"></script>
  <style>
    body { background: #f9f9f9; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif; }
    .center { text-align: center; margin-top: 2em; }
  </style>
</head>
<body>
  <ons-page>
    <ons-toolbar>
      <div class="center">OTA & LCD Control</div>
    </ons-toolbar>
    <div style="margin: 20px;">
      <ons-card>
        <div class="title">OTA Update</div>
        <form method="POST" action="/update" enctype="multipart/form-data">
          <input type="file" name="update">
          <ons-button type="submit">Update</ons-button>
        </form>
      </ons-card>
      <ons-card>
        <div class="title">LCD Message</div>
        <form action="/lcd" method="POST">
          <ons-input name="msg" placeholder="Type text for LCD" modifier="underbar"></ons-input>
          <ons-button type="submit">Play to LCD</ons-button>
        </form>
      </ons-card>
    </div>
  </ons-page>
</body>
</html>
)=====";

#endif