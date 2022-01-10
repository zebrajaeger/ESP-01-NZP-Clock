// 128x64 OLED pinout:
// GND goes to ground
// Vin goes to 3.3V
// Data to I2C SDA (GPIO 0)
// Clk to I2C SCL (GPIO 2)

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WifiUDP.h>

#include <Wire.h>
#include <SSD1306.h>

#include "config.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS);

HTTPClient sender;
WiFiClient wifiClient;

SSD1306Wire  display(0x3c, 0, 2);

void initWifi() {
  Serial.print("Verbinden mit: ");
  Serial.println(ssid);
   display.drawString(0, 10, "Connecting to Wifi...");
   display.display();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi verbunden: ");
  Serial.println(WiFi.localIP());
  display.drawString(0, 24, "Connected.");
  display.display();
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  display.init();
  display.flipScreenVertically();

  initWifi();

  timeClient.begin();
}

char *toTimeString(time_t local, char *buffer) {
  snprintf(buffer, 6, "%02d:%02d", hour(local), minute(local));
  return buffer;
}

char *toDateString(time_t local, char *buffer) {
  snprintf(buffer, 31, "%s, %02d.%02d.%04d", days[weekday(local) - 1],
           day(local), month(local), year(local));
  return buffer;
}

void print2Display(char *t, char *d) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawStringMaxWidth(64, 10, 128, t);
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(64, 38, 128, d);
  display.display();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED)
  {
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    Serial.println(epochTime);

    time_t local, utc;
    utc = epochTime;
    Timezone TZ(CEST, CET);
    local = TZ.toLocal(utc);
    Serial.println("----");
    char t[16];
    char d[32];
    Serial.println(toTimeString(local, t));
    Serial.println(toDateString(local, d));
    print2Display(t, d);
  } else {
    initWifi();
  }
  delay(10000);
}

//
//void scani2c() {
//  char s[128];
//  for (int i = 1; i < 128; ++i) {
//    Wire.beginTransmission(i);
//    bool error = Wire.endTransmission();
//
//    snprintf(s, 128, "http://192.168.178.28/%d/%s", i,
//             error == 0 ? "JA" : "nein");
//    Serial.println(s);
//    if (sender.begin(wifiClient, s)) {
//      int httpCode = sender.GET();
//      Serial.println(httpCode);
//      sender.end();
//    } else {
//      Serial.println("failed");
//    }
//    delay(500);
//  }
//}
