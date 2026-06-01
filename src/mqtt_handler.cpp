#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include "config.h"
#include "globals.h"
#include "display.h"

static WiFiClient espClient;
static PubSubClient client(espClient);

static void update_top_ram_container(const char* container_name, float usage_bytes) {
  if (container_name == nullptr || container_name[0] == '\0') return;

  // Convert bytes to MB
  float usage_mb = usage_bytes / (1024.0 * 1024.0);

  // Update existing container entry if already present
  for (int i = 0; i < 3; i++) {
    if (top_ram_containers[i].name == container_name) {
      top_ram_containers[i].ram_mb = usage_mb;
      // Bubble up
      for (int j = i; j > 0; j--) {
        if (top_ram_containers[j].ram_mb > top_ram_containers[j - 1].ram_mb) {
          ContainerRam tmp = top_ram_containers[j - 1];
          top_ram_containers[j - 1] = top_ram_containers[j];
          top_ram_containers[j] = tmp;
        } else {
          break;
        }
      }
      return;
    }
  }

  // If this container is not in the top list and it's too small, ignore it
  if (usage_mb <= top_ram_containers[2].ram_mb) {
    return;
  }

  // Insert into bottom and bubble up
  top_ram_containers[2].name = String(container_name);
  top_ram_containers[2].ram_mb = usage_mb;
  for (int j = 2; j > 0; j--) {
    if (top_ram_containers[j].ram_mb > top_ram_containers[j - 1].ram_mb) {
      ContainerRam tmp = top_ram_containers[j - 1];
      top_ram_containers[j - 1] = top_ram_containers[j];
      top_ram_containers[j] = tmp;
    } else {
      break;
    }
  }
}

static void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // parse payload into string for debug (only if DEBUG)
  if (DEBUG) {
    String pacote = "";
    for (unsigned int i = 0; i < length; i++) pacote += (char)payload[i];
    Serial.println("[MQTT] Mensagem recebida: ");
    Serial.println(pacote);
  }

  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print("Erro ao ler JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* nome_metrica = doc["name"];
  if (!nome_metrica) return;

  if (strcmp(nome_metrica, "cpu") == 0) {
    float usage_idle = doc["fields"]["usage_idle"] | 0.0;
    uso_cpu = 100.0 - usage_idle;
  } else if (strcmp(nome_metrica, "mem") == 0) {
    uso_ram = doc["fields"]["used_percent"] | 0.0;
  } else if (strcmp(nome_metrica, "disk") == 0) {
    const char* caminho = doc["tags"]["path"] | "";
    float usedPercent = doc["fields"]["used_percent"] | 0.0;
    if (strcmp(caminho, "/") == 0) {
      uso_disco = usedPercent;
      disk_path = "/";
    } else if (disk_path.length() == 0) {
      uso_disco = usedPercent;
      disk_path = String(caminho);
    }
  } else if (strcmp(nome_metrica, "system") == 0) {
    if (doc["fields"].containsKey("uptime")) {
      uptime_servidor = doc["fields"]["uptime"] | 0;
    }
  }
  // container memory
  else if (strcmp(nome_metrica, "docker_container_mem") == 0) {
    const char* nome_container = doc["tags"]["container_name"] | "";
    float usage_bytes = doc["fields"]["usage"] | 0.0;
    update_top_ram_container(nome_container, usage_bytes);
  }
}

void mqtt_connect_and_subscribe() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Monitor-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao MQTT");
      client.subscribe("servidor/metricas");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" — tentando novamente em 5s");
      delay(5000);
    }
  }
}

void mqtt_setup() {
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(mqtt_callback);
  client.setBufferSize(4096);
}

void mqtt_loop() {
  if (!client.connected()) {
    mqtt_connect_and_subscribe();
  }
  client.loop();
  // always print a concise dashboard to serial (helpful for human-readable monitoring)
  static unsigned long last = 0;
  if (millis() - last >= 2000) {
    last = millis();
    display_render_serial();
  }
}
