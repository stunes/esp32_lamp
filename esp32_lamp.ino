// Copyright 2017 Mike Stunes.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "secret.h"

const char *GROUP_URL  = "http://" BRIDGE "/api/" API_USERNAME "/groups/" ROOM;
const char *ACTION_URL = "http://" BRIDGE "/api/" API_USERNAME "/groups/" ROOM "/action";

const String SCENE_DATA = "{\"scene\": \"" SCENE "\"}";
const String OFF_DATA = "{\"on\": false}";

const int INPUT_PIN = 12;
const int LED_PIN = 22;

const unsigned int BOUNCE_DELAY_MS = 500; // ms

unsigned long lastInterrupt;  // last interrupt time
volatile int shouldTrigger = 0;

// connectToWiFi adapted from ESP32 example code. See, e.g.:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClient/WiFiClient.ino
void connectToWiFi() {
  Serial.println("Connecting to: " + String(SSID));
  int ledState = 0;

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, ledState);
    ledState = (ledState + 1) % 2;
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_PIN, HIGH);

  Serial.println();
  Serial.println("Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void putJson(const char *url, String content) {
  Serial.println("putJson");
  Serial.printf("PUT %s: %s\n", url, content.c_str());

  HTTPClient http;
  http.begin(url);
  int httpCode = http.PUT(content);
  if (httpCode > 0) {
    Serial.printf("Code: %d\n", httpCode);
  } else {
    Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
  }
  Serial.println(http.getString());
  http.end();
}

String getUrl(const char *url) {
  Serial.println("get");
  Serial.printf("GET %s\n", url);

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.printf("Code: %d\n", httpCode);
  } else {
    Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
  }

  return http.getString();
}

void turnLightsOn() {
  Serial.println("turnLightsOn");
  putJson(ACTION_URL, SCENE_DATA);
}

void turnLightsOff() {
  Serial.println("turnLightsOff");
  putJson(ACTION_URL, OFF_DATA);
}

bool lightsOn() {
  Serial.println("lightsOn");
  String jsonBody = getUrl(GROUP_URL);

  StaticJsonBuffer<4096> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(jsonBody);
  bool isOn = root["state"]["any_on"];
  Serial.printf("isOn: %d\n", isOn);
  return isOn;
}

void toggleLights() {
  if (lightsOn()) {
    turnLightsOff();
  } else {
    turnLightsOn();
  }
}

void handleButton() {
  unsigned long currentTime = millis();
  if ((currentTime - lastInterrupt) > BOUNCE_DELAY_MS) {
    Serial.println("Handling button event");
    lastInterrupt = currentTime;
    shouldTrigger = 1;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  pinMode(LED_PIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);

  connectToWiFi();

  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), handleButton, FALLING);
  Serial.println("Button interrupt enabled");
}

void loop() {
  if (shouldTrigger) {
    toggleLights();
    shouldTrigger = 0;
  }
}
