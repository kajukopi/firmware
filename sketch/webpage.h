#ifndef WEBPAGE_H
#define WEBPAGE_H

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP8266 Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsenui.css">
  <link rel="stylesheet" href="https://unpkg.com/onsenui/css/onsen-css-components.min.css">
  <script src="https://unpkg.com/onsenui/js/onsenui.min.js"></script>
</head>
<body>
<ons-page>
  <ons-splitter>
    <ons-splitter-side side="left" width="220px" collapse swipeable>
      <ons-page>
        <ons-list>
          <ons-list-item onclick