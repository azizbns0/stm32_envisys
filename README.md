# STM32 Environmental Monitoring System

[![License](https://img.shields.io/badge/License-SLA0044-blue.svg)](https://www.st.com/SLA0044)
[![Platform](https://img.shields.io/badge/Platform-STM32F4-blue)](https://www.st.com/en/microcontrollers-microprocessors/stm32f4-series.html)

A real-time embedded system for monitoring environmental data (temperature, pressure, altitude) using the **BMP180 sensor** and displaying results on an **SSD1306 OLED**. Built with STM32CubeIDE and HAL libraries.

---

## Table of Contents
- [Features](#features)
- [Hardware Components](#hardware-components)
- [Setup](#setup)
- [Configuration](#configuration)
- [Workflow](#workflow)
- [Current Limitations](#current-limitations)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Features
- **Non-blocking operation**: Uses DMA and timer interrupts for efficient task scheduling.
- **Real-time data**: 
  - Temperature (°C)
  - Pressure (Pa)
  - Altitude (m)
- **OLED Display**: Cyclic display of data on SSD1306 (128x64 pixels).
- **UART Debugging**: Data transmission via USART3 (9600 baud).

---

## Hardware Components
| Component             | Connection                   | Description                     |
|-----------------------|------------------------------|---------------------------------|
| STM32F4 Discovery     | -                            | Main microcontroller           |
| BMP180 Sensor         | I2C1 (SCL: PB6, SDA: PB7)    | Barometric pressure sensor     |
| SSD1306 OLED          | I2C1 (SCL: PB6, SDA: PB7)    | 0.96" OLED display             |
| USB-UART Converter    | USART3 (TX: PD8, RX: PD9)    | For serial debugging           |



## Setup

### Prerequisites
- STM32CubeIDE
- STM32F4 Discovery Board
- BMP180 and SSD1306 modules
- USB-UART converter (e.g., FTDI)

### Installation
1. **Clone the repository**:
   ```bash
   git clone https://github.com/azizbns0/stm32_project1.git
