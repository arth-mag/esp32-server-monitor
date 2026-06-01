#include <Arduino.h>
#include "globals.h"

float uso_cpu = 0.0;
float uso_ram = 0.0;
float uso_disco = 0.0;
long uptime_servidor = 0;
String disk_path = "";
ContainerRam top_ram_containers[3] = {{String(""), 0.0f}, {String(""), 0.0f}, {String(""), 0.0f}};
