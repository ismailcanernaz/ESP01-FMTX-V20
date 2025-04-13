/**
  @file    esp01_fmtx_demo.ino
  @author  esp8266-www.elechouse.com
  @brief   example of FMTX20_MODULE
  
    For this demo, input command format like &xxxx to set select band,
 (eg:if you want to set FM 95.6MHz,then input &0956 and send).
  
  @section  HISTORY
  
  V2.0     esp01 initial version
  
    Copyright (c) 2025 esp8266 www.elechouse.com  All right reserved.
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "FMTX20.h"

const char* ssid = "ssid";
const char* password = "password";

#define SDA 0
#define SCL 2

ESP8266WebServer server(80);

float fm_freq = 95.5;

void handleRoot() {
  fm_freq = fmtx_get_freq();
  int rfpow = fmtx_get_rfgain();
  bool stereo = fmtx_is_stereo();
  bool silence = fmtx_get_silence();
  bool muted = fmtx_get_mute();

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<title>FMTX V2.0 DEMO</title>";
  html += "<style>body{font-family:sans-serif;padding:20px;}input,select{width:100px;}h1{color:#444;}label{display:block;margin-top:10px;}</style>";
  html += "</head><body>";
  html += "<h1>ELECHOUSE</h1>";
  html += "<form action='/set' method='GET'>";
  
  // Frequency
  html += "<label for='freq'>70-108MHz</label>";
  html += "<input type='number' step='0.1' min='70.0' max='108.0' name='freq' id='freq'>";
  
  // RF Gain
  html += "<label for='rfpow'>TX Power</label>";
    html += "<select name='rfpow' id='rfpow'>";
    html += "<option value='0'>Low</option>";
    html += "<option value='1'>Medium</option>";
    html += "<option value='2'>High</option>";
    html += "<option value='3'>Max</option>";
    html += "</select>";
  
  // Country (EU/USA)
  html += "<label for='country'>Country</label>";
  html += "<select name='country' id='country'>";
  html += "<option value='0'>EU-AUS-CHI</option>";
  html += "<option value='1'>USA-JAP</option>";
  html += "</select>";

  // PGA Gain
  html += "<label for='pga'>Pre-Amp</label>";
  html += "<select name='pga' id='pga'>";
  html += "<option value='4'>0 dB</option>";
  html += "<option value='5'>+4 dB</option>";
  html += "<option value='6'>+8 dB</option>";
  html += "<option value='7'>+12 dB</option>";
  html += "<option value='0'>-0 dB</option>";
  html += "<option value='1'>-4 dB</option>";
  html += "<option value='2'>-8 dB</option>";
  html += "<option value='3'>-12 dB</option>";
  html += "</select>";

  // Mute checkbox
  html += "<label for='mute'>Mute</label>";
  html += "<input type='checkbox' name='mute' id='mute' value='1'" + String(muted ? " checked" : "") + ">";

  html += "<br><br><input type='submit' value='Set'>";
  html += "</form>";

  // Status
  html += "<h2>FM TRANSMITTER V2.0</h2>";
  html += "<h1>Status</h1>";
  html += "<p><strong>Frequency:</strong> " + String(fm_freq, 1) + " MHz</p>";
  html += "<p><strong>RF Gain:</strong> " + String(rfpow) + "</p>";
  html += "<p><strong>Stereo:</strong> " + String(stereo ? "Yes" : "No") + "</p>";
  html += "<p><strong>Silence:</strong> " + String(silence ? "Yes" : "No") + "</p>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSetFreq() {
  if (server.hasArg("freq")) {
    float newFreq = server.arg("freq").toFloat();
    if (newFreq >= 70.0 && newFreq <= 108.0) {
      fmtx_set_freq(newFreq);
      delay(50);
    }
  }

  if (server.hasArg("rfpow")) {
    int gain = server.arg("rfpow").toInt();
    fmtx_set_rfgain((fmtx_rfgain_type)gain);
  }

  if (server.hasArg("country")) {
    int country = server.arg("country").toInt();
    fmtx_set_phcnst((country == 0) ? EUROPE : USA);
  }

  if (server.hasArg("pga")) {
    int pga = server.arg("pga").toInt();
    fmtx_set_pga((fmtx_pga_type)pga);
  }

  if (server.hasArg("mute")) {
    fmtx_set_mute(true);
  } else {
    fmtx_set_mute(false);
  }

  handleRoot();
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected. IP address: ");
  Serial.println(WiFi.localIP());

  Wire.begin(SDA, SCL);
  Wire.setClock(I2C_FREQ);

  fmtx_init(fm_freq, EUROPE);

  server.on("/", handleRoot);
  server.on("/set", handleSetFreq);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
