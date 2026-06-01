#pragma once

#include <Arduino.h>

struct ContainerRam {
  String name;
  float ram_mb; // RAM em MB
};

extern float uso_cpu;
extern float uso_ram;
extern float uso_disco;
extern long uptime_servidor;
extern String disk_path;
extern ContainerRam top_ram_containers[3];
