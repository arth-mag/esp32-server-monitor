#include <Arduino.h>
#include "config.h"
#include "wifi.h"
#include "mqtt_handler.h"
#include "display.h"

unsigned long tempoAnterior = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\nIniciando monitor do servidor...");

  wifi_init();
  delay(2000); // Aguarda WiFi estabilizar
  mqtt_setup();
  display_init();
}

void loop() {
  mqtt_loop();

  if (millis() - tempoAnterior >= 2000) {
    tempoAnterior = millis();
    display_render();
  }
}