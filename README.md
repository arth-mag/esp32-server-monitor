# ESP32 Monitor ZimaOS

Monitor em tempo real para servidor ZimaOS via display ST7735 e WiFi/MQTT.

## Visão Geral

Aplicação embarcada em **ESP32** que se conecta ao servidor **ZimaOS** via WiFi e recebe métricas do servidor (CPU, RAM, disco, uptime) e dos containers Docker via protocolo **MQTT** do Telegraf. Exibe as informações em um **display TFT ST7735 1.8"** e também no serial (115200 baud).

### Arquitetura

```
┌──────────────────────────────────────────────┐
│           ESP32 + ST7735 Display             │
├──────────────────────────────────────────────┤
│  WiFi                                        │
│    └─ Arthur2.4_ (SSID)                     │
│                                              │
│  MQTT (PubSubClient)                        │
│    └─ Broker: 192.168.2.21:1883            │
│    └─ Tópico: servidor/metricas            │
│                                              │
│  Parsing JSON (ArduinoJson)                 │
│    └─ Telegraf metrics (cpu, mem, disk...) │
│    └─ Docker container metrics             │
│                                              │
│  Display (Adafruit ST7735)                  │
│    └─ SPI: SCK=18, MOSI=23                 │
│    └─ CS=5, DC=16, RST=17                  │
│    └─ 128x160 pixels                        │
│                                              │
│  Serial Output                              │
│    └─ Resumo legível a cada 2s             │
└──────────────────────────────────────────────┘
```

## Estrutura do Projeto

```
src/
├── main.cpp                 # Orquestrador: setup() e loop()
├── globals.cpp              # Variáveis globais de estado
├── wifi.cpp                 # Inicialização e gerenciamento WiFi
├── mqtt_handler.cpp         # Parsing MQTT + callback JSON
└── display_st7735.cpp       # Renderização display + serial

include/
├── config.h                 # Configurações (WiFi, MQTT, pinos, DEBUG)
├── globals.h                # Declaração de globais e structs
├── wifi.h                   # Interface WiFi
├── mqtt_handler.h           # Interface MQTT
└── display.h                # Interface display

lib/                         # (vazio — libs no platformio.ini)
test/                        # (vazio)
```

## Dependências

**PlatformIO (ESP32 Arduino Framework)**

- `espressif32` — platform ESP32
- `esp32dev` — board ESP32 Dev Module

**Bibliotecas C++**

| Biblioteca | Versão | Uso |
|---|---|---|
| `PubSubClient` | ^2.8 | Cliente MQTT |
| `ArduinoJson` | ^7.2.2 | Parsing JSON |
| `Adafruit GFX Library` | ^1.11.5 | Gráficos base |
| `Adafruit ST7735 and ST7789 Library` | ^1.6.1 | Driver display SPI |

Instaladas automaticamente pelo PlatformIO via `platformio.ini`.

## Compilação e Upload

### Requisitos

- [PlatformIO CLI](https://platformio.org/install/cli)
- Python 3.6+
- USB/Serial para ESP32

### Build

```bash
cd /home/arthmag/Documentos/PlatformIO/Projects/Monitor

# Build apenas (sem upload)
pio run --environment esp32dev

# Build + Upload
pio run -t upload -e esp32dev

# Monitor serial
pio device monitor -b 115200
```

### Configuração

Edite `include/config.h` para ajustar:

```cpp
static const char* WIFI_SSID = "Arthur2.4_";
static const char* WIFI_PASSWORD = "@moNina08anos";
static const char* MQTT_SERVER = "192.168.2.21";  // IP do ZimaOS/Mosquitto

// Display ST7735 pinos SPI
static const int TFT_CS = 5;   // Chip select
static const int TFT_DC = 16;  // Data/Command
static const int TFT_RST = 17; // Reset

// Debug: true = imprime pacotes MQTT brutos
static const bool DEBUG = false;
```

**Fiação recomendada (ESP32 → ST7735)**

| ESP32 | ST7735 | Função |
|---|---|---|
| 18 | CLK | Clock SPI |
| 23 | MOSI | Data SPI |
| 5 | CS | Chip Select |
| 16 | DC | Data/Command |
| 17 | RST | Reset |
| GND | GND | Ground |
| 3.3V | VCC | Power |

## Funcionamento

### Fluxo de Dados

1. **WiFi Connect**: ESP32 conecta a `Arthur2.4_` (SSID)
2. **MQTT Connect**: Conecta ao broker em `192.168.2.21:1883`
3. **Subscribe**: Inscreve-se no tópico `servidor/metricas`
4. **Telegram JSON Receive**: Recebe pacotes JSON do Telegraf contendo métricas do servidor
5. **Parse**: Extrai campos relevantes (CPU, RAM, disco, uptime, containers)
6. **Update State**: Atualiza variáveis globais
7. **Display Render**: A cada 2s, renderiza display TFT e imprime resumo no serial

### Métricas Capturadas

**Sistema (ZimaOS)**

| Métrica | Campo JSON | Unidade | Fonte |
|---|---|---|---|
| CPU Usage | `cpu` → `fields.usage_idle` | % | Telegraf CPU |
| RAM Usage | `mem` → `fields.used_percent` | % | Telegraf Mem |
| Disk (/) | `disk` → `fields.used_percent` (path=/) | % | Telegraf Disk |
| Uptime | `system` → `fields.uptime` | segundos | Telegraf System |

**Containers Docker**

| Métrica | Campo JSON | Unidade | Fonte |
|---|---|---|---|
| Container RAM | `docker_container_mem` → `fields.usage` | bytes | Telegraf Docker |

**Top 3 Containers (por RAM)**

Ranking em tempo real dos 3 containers que mais consomem RAM (em MB). Atualizado sempre que nova métrica `docker_container_mem` chega.

### Saída Serial

A cada 2 segundos:

```
====== MONITOR ZIMAOS ======
CPU: 7.9 %  RAM: 35.5 %  DISCO(/): 5.8 %
Uptime: 4h 4m
Top 3 RAM containers:
1) radarr 161 MB
2) emby 256 MB
3) plex 512 MB
============================
```

### Display ST7735

Layout (128x160 pixels):

```
┌──────────────────────┐
│  MONITOR ZIMAOS      │  ← Título
├──────────────────────┤
│  CPU: 7.9 %          │
│  RAM: 35.5 %         │
│  DISCO(/): 5.8 %     │
│  UP: 4h 4m           │
│  Top RAM containers: │
│  1 radarr 161MB      │
│  2 emby 256MB        │
│  3 plex 512MB        │
└──────────────────────┘
```

## Módulos

### `src/main.cpp`

Orquestrador principal. Inicializa WiFi → MQTT → Display e loop. Atualiza display TFT a cada 2s.

```cpp
void setup() {
  wifi_init();              // Conecta WiFi
  delay(2000);              // Aguarda estabilizar
  mqtt_setup();             // Inicializa MQTT client
  display_init();           // Inicializa display SPI
}

void loop() {
  mqtt_loop();              // Processa MQTT + imprime serial dashboard
  if (millis() - tempoAnterior >= 2000) {
    tempoAnterior = millis();
    display_render();       // Atualiza display TFT
  }
}
```

### `src/wifi.cpp` / `include/wifi.h`

Gerencia conexão WiFi com retry automático.

```cpp
void wifi_init();  // Conecta ao SSID configurado
```

### `src/mqtt_handler.cpp` / `include/mqtt_handler.h`

Processa MQTT e parsing JSON.

**Callback estático**: Recebe pacote JSON do broker, deserializa com `ArduinoJson` e atualiza globais:
- CPU, RAM, Disco (métricas sistema)
- Uptime (se presente)
- Top 3 containers (via `update_top_ram_container()`)

**Interface:**

```cpp
void mqtt_setup();        // Configura PubSubClient
void mqtt_loop();         // Mantém conexão viva + chama callback
                          // + imprime dashboard serial a cada 2s
```

### `src/display_st7735.cpp` / `include/display.h`

Renderização TFT e Serial.

```cpp
void display_init();           // Inicializa SPI + ST7735
void display_render();         // Desenha no display TFT
void display_render_serial();  // Imprime dashboard legível no serial
```

### `src/globals.cpp` / `include/globals.h`

Estado global compartilhado entre módulos.

```cpp
float uso_cpu;                       // % (0-100)
float uso_ram;                       // % (0-100)
float uso_disco;                     // % (0-100)
long uptime_servidor;                // segundos
String disk_path;                    // caminho disco detectado
ContainerRam top_ram_containers[3];  // top 3 containers (nome, RAM em MB)

struct ContainerRam {
  String name;      // Nome container
  float ram_mb;     // RAM em MB
};
```

### `include/config.h`

Centralizador de configurações:
- WiFi (SSID, password, broker IP)
- Display (pinos SPI)
- Debug (ativa logs MQTT brutos)

## Troubleshooting

### WiFi conecta mas MQTT falha (rc=-2)

**Causa**: ESP32 em rede WiFi diferente do servidor MQTT.

**Solução**: Confirme que ESP32 conectou na mesma rede (192.168.2.x para servidor em 192.168.2.21) e atualize `MQTT_SERVER` em `config.h`.

### Display não mostra nada

**Causa**: Pinos SPI incorretos ou display não inicializa.

**Solução**: 
1. Verifique fiação (CS, DC, RST)
2. Ajuste pinos em `config.h`
3. Tente diferentes inicializações: `initR(INITR_144GREENTAB)` vs `INITR_18BLACKTAB`

### Uptime zerado

**Causa**: Múltiplas métricas "system" — nem todas vêm com uptime.

**Solução**: Código agora verifica se campo "uptime" existe antes de atualizar (✅ corrigido).

### Top 3 RAM mostra valores estranhos

**Causa**: Campo JSON incorreto ou tipo de dado errado.

**Solução**: Verifique que `docker_container_mem` contém campo `usage` (bytes). Se for outro nome, ajuste em `mqtt_handler.cpp`.

## Debug

Para ativar log bruto de pacotes MQTT:

```cpp
// include/config.h
static const bool DEBUG = true;  // ← mude para true
```

Recompile e upload. Serial exibirá cada pacote JSON recebido.

## Licença

Projeto pessoal. Sinta-se livre para adaptar e estender.

## Roadmap Futuro

- [ ] Paginação display (2+ telas)
- [ ] Alertas por threshold (CPU/RAM crítico)
- [ ] Salvar configurações em EEPROM
- [ ] WebUI para reconfiguração WiFi
- [ ] Gráfico histórico (últimas 10 métricas)
- [ ] Suporte múltiplos discos

---

**Última atualização**: Maio 2026  
**Status**: ✅ Funcional
