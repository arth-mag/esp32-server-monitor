ESP32 Homelab Telemetry Dashboard

A high-performance, low-latency telemetry monitoring system designed to visualize infrastructure metrics in real-time. This project bridges the gap between server-side data and embedded visualization using the MQTT protocol.
🚀 Project Overview

The core objective of this project was to build a robust observability solution for my personal homelab, eliminating the need for heavy web interfaces for quick system health checks. The system processes asynchronous data streams from Docker and the OS, filtering and displaying critical metrics with minimal latency on an embedded ST7735 display.

🏗️ System Architecture

The system is decoupled, adhering to modular software design principles:

    Data Acquisition: Telegraf acts as the telemetry agent on the host (ZimaOS), scraping CPU, RAM, Disk, and Docker container states via the local socket (/var/run/docker.sock).

    Messaging Layer: Mosquitto MQTT broker manages the message bus, ensuring lightweight and efficient data delivery.

    Edge Processing: The ESP32 employs an event-driven architecture to perform on-the-fly JSON deserialization and pixel manipulation on the local display.

⚙️ Technical Challenges & Solutions

    Docker Daemon Security: Accessing the Docker Socket from within a container required careful management of runtime privileges and namespace isolation, resolved through rigorous configuration of service user groups and filesystem permissions.

    Embedded Memory Management: Handling complex JSON payloads (often >1KB due to Docker metadata) required scaling the MQTT_BUFFER_SIZE to 4096 bytes. This was critical to prevent packet fragmentation and data loss within the PubSubClient.

    Modular Software Design: The firmware was refactored from a monolithic script into a component-based structure using PlatformIO. By decoupling communication logic (mqtt_handler) from hardware drivers (display_st7735), the codebase achieves high maintainability and hardware portability.

🛠️ Technology Stack

    Microcontroller: ESP32-WROOM-32.

    Communication: MQTT (Publish/Subscribe), Wi-Fi (802.11 b/g/n).

    Data Processing: ArduinoJson (v7) for efficient deserialization.

    Infrastructure: Docker, Telegraf, Mosquitto MQTT Broker.

    Build System: PlatformIO (VS Code).

📊 Repository Structure
Plaintext

├── src/
│   ├── main.cpp          # Entry point and state machine logic
│   ├── mqtt_handler.cpp  # MQTT connection management & JSON parsing
│   └── display_st7735.cpp # Hardware abstraction layer for the display
├── include/              # Header files and interface definitions
├── platformio.ini        # Build configurations and dependency management
