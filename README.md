ESP32 Server Monitor

A personal project aimed at understanding the lifecycle of telemetry data, from low-level system metrics to real-time visualization on an embedded device.
The Journey

This project was born out of a simple curiosity: "How can I visualize my ZimaOS homelab status on my desk without relying on a web browser?" What started as a simple exploration of MQTT protocols evolved into a deep dive into Linux telemetry, Docker internals, and embedded C++ optimization.
Technical Challenges & Learnings

    The Telemetry Pipeline: Learning how to use Telegraf as a data collector and Mosquitto as the MQTT broker to bridge server-side data with the physical world.

    Docker Socket Security: Overcoming permission denied errors when accessing the Docker daemon from within a container—a crash course in container security, user namespaces, and runtime privileges.

    JSON Handling on Embedded: Wrestling with ArduinoJson and memory buffers. Scaling the MQTT buffer from 256 bytes to 4096 bytes was a crucial realization in handling "heavy" JSON payloads from Docker status updates.

    Clean Code Architecture: Migrating from a monolithic script to a modular structure using PlatformIO, separating concerns into mqtt_handler, display_st7735, and wifi modules to improve maintainability.

Architecture

    Server Side: ZimaOS hosting a Telegraf container that scrapes metrics (CPU, RAM, Disk, Docker status).

    Messaging: Mosquitto MQTT broker acting as the central hub.

    Client Side: ESP32 micro-controller performing real-time JSON deserialization and pixel manipulation on an ST7735 display.

Project Structure

    src/: Core logic, including MQTT message handling and display drivers.

    include/: Header files and global definitions (check config.h.example to set up your environment).

    platformio.ini: Project dependencies and build configuration.

Getting Started

    Infrastructure: Ensure you have your Telegraf/MQTT stack running on your ZimaOS.

    Configuration: Copy include/config.h.example to include/config.h and fill in your network and broker credentials.

    Build: Use PlatformIO to compile and upload the project to your ESP32.
