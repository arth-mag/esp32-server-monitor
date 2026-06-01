#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Arduino.h>
#include "config.h"
#include "globals.h"

// Create display object (hardware SPI)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void display_init() {
  SPI.begin();
  tft.initR(INITR_144GREENTAB); // try a common init; adjust if colors/sizes differ
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(1);
  tft.setTextWrap(false);
}

void display_render() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.print("MONITOR ZIMAOS");

  tft.setCursor(0, 16);
  tft.print("CPU: "); tft.print(uso_cpu, 1); tft.print(" %");

  tft.setCursor(0, 32);
  tft.print("RAM: "); tft.print(uso_ram, 1); tft.print(" %");

  tft.setCursor(0, 48);
  tft.print("DISCO(");
  tft.print(disk_path.length() ? disk_path : String("?"));
  tft.print("): ");
  tft.print(uso_disco, 1); tft.print(" %");

  tft.setCursor(0, 64);
  long uptime_h = uptime_servidor / 3600;
  long uptime_m = (uptime_servidor % 3600) / 60;
  tft.print("UP: "); tft.print(uptime_h); tft.print("h "); tft.print(uptime_m); tft.print("m");

  tft.setCursor(0, 80);
  tft.print("Top RAM containers:");

  tft.setCursor(0, 96);
  if (top_ram_containers[0].name.length()) {
    tft.print("1 "); tft.print(top_ram_containers[0].name);
    tft.print(" "); tft.print(top_ram_containers[0].ram_mb, 0); tft.print("MB");
  }

  tft.setCursor(0, 112);
  if (top_ram_containers[1].name.length()) {
    tft.print("2 "); tft.print(top_ram_containers[1].name);
    tft.print(" "); tft.print(top_ram_containers[1].ram_mb, 0); tft.print("MB");
  }

  tft.setCursor(0, 128);
  if (top_ram_containers[2].name.length()) {
    tft.print("3 "); tft.print(top_ram_containers[2].name);
    tft.print(" "); tft.print(top_ram_containers[2].ram_mb, 0); tft.print("MB");
  }
}

void display_render_serial() {
  Serial.println("====== MONITOR ZIMAOS ======");
  Serial.print("CPU: "); Serial.print(uso_cpu, 1); Serial.print(" %  ");
  Serial.print("RAM: "); Serial.print(uso_ram, 1); Serial.print(" %  ");
  Serial.print("DISCO("); Serial.print(disk_path.length() ? disk_path : String("?")); Serial.print("): ");
  Serial.print(uso_disco, 1); Serial.print(" %\n");

  long uptime_h = uptime_servidor / 3600;
  long uptime_m = (uptime_servidor % 3600) / 60;
  Serial.print("Uptime: "); Serial.print(uptime_h); Serial.print("h "); Serial.print(uptime_m); Serial.println("m");
  Serial.println("Top 3 RAM containers:");
  for (int i = 0; i < 3; i++) {
    if (top_ram_containers[i].name.length()) {
      Serial.print(i + 1);
      Serial.print(") ");
      Serial.print(top_ram_containers[i].name);
      Serial.print(" ");
      Serial.print(top_ram_containers[i].ram_mb, 0);
      Serial.println(" MB");
    }
  }
  Serial.println("============================\n");
}
