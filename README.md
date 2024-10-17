# Environmental Monitoring System

## Project Description

The Environmental Monitoring System utilizes a BMP180 sensor to measure temperature, pressure, and altitude, displaying real-time data on an OLED screen using an STM32 microcontroller. The project also includes UART communication to transmit data to an ESP32 for additional functionalities, such as cloud connectivity or remote monitoring.

## Features

- **Real-time Environmental Data**: Continuously captures temperature, pressure, and altitude readings.
- **OLED Display**: Presents sensor data in a user-friendly format.
- **UART Communication**: Sends data to an ESP32 for further processing and potential web integration.
- **Modular Design**: Easily extendable to accommodate additional sensors or features.

## Hardware Requirements

- STM32F407 Microcontroller
- BMP180 Barometric Sensor
- SSD1306 OLED Display
- ESP32 (optional for future enhancements)
- Jumper wires and breadboard for connections

## Software Requirements

- STM32CubeIDE
- HAL library for STM32
- C programming language
