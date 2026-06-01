#pragma once

// Rede e MQTT
static const char* WIFI_SSID = "Arthur2.4_";
static const char* WIFI_PASSWORD = "@moNina08anos";
static const char* MQTT_SERVER = "192.168.2.21";

// Display ST7735 (pinos sugeridos — adapte conforme seu wiring)
// Pinos SPI padrão ESP32: SCK=18, MOSI=23
static const int TFT_CS = 5;   // Chip select
static const int TFT_DC = 16;  // Data/Command
static const int TFT_RST = 17; // Reset

// Debug
// Set to true to enable raw MQTT packet prints (verbose). Otherwise only concise dashboard is printed.
static const bool DEBUG = false;
