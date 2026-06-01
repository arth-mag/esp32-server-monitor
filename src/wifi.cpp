#include <WiFi.h>
#include <Arduino.h>
#include "config.h"

void wifi_init() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
    if (millis() - start > 20000) {
      Serial.println("\nFalha ao conectar Wi-Fi — tentando novamente");
      start = millis();
    }
  }
  Serial.println("\nWi-Fi Conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
}
