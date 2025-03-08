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
- [DMA Usage](#dma-usage)
- [Current Limitations](#current-limitations)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Features
- **Non-blocking operation**: Uses DMA and timer interrupts for efficient task scheduling.
- **Real-time data acquisition**: 
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

---

## Setup

### Prerequisites
- STM32CubeIDE
- STM32F4 Discovery Board
- BMP180 and SSD1306 modules
- USB-UART converter (e.g., FTDI)

### Installation
1. **Clone the repository**:
   ```bash
   git clone https://github.com/azizbns0/stm32_envisys.git
   ```
2. **Open in STM32CubeIDE**:
   - Import the project: `File > Import > Existing Projects into Workspace`.
3. **Build and Flash**:
   - Build the project (`Ctrl+B`).
   - Flash to the microcontroller (`Run > Debug`).

---

## Configuration

### Timer Interrupt (TIM2)
| Parameter         | Value   | Description                  |
|------------------|--------|------------------------------|
| Clock Source     | 84 MHz | Derived from APB1 x2         |
| Prescaler (PSC)  | 8399   | Timer clock = 10 kHz         |
| Counter Period   | 999    | Interrupt every 100 ms       |

#### Tasks:
- **Sensor Read**: Every 100 ms (`read_sensor_flag`).
- **Display Update**: Every 500 ms (`update_display_flag`).

---

## Workflow

### Initialization:
```c
HAL_TIM_Base_Start_IT(&htim2);  // Start timer interrupt
BMP180_Start();                 // Initialize sensor
SSD1306_Init();                 // Initialize display
```

### Timer Interrupt:
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        static uint16_t counter = 0;
        counter++;
        if (counter % 100 == 0) read_sensor_flag = 1;    // 100 ms
        if (counter % 500 == 0) update_display_flag = 1;  // 500 ms
    }
}
```

### Main Loop:
```c
while (1) {
    if (read_sensor_flag) {
        // Read sensors and send data via UART
        Temperature = BMP180_GetTemp();
        Pressure = BMP180_GetPress(0);
        Altitude = BMP180_GetAlt(0);
        char buffer[100];
        sprintf(buffer, "Temperature: %.2f C, Pressure: %.2f Pa, Altitude: %.2f m\n\r", 
                Temperature, Pressure, Altitude);
        HAL_UART_Transmit_DMA(&huart3, (uint8_t*)buffer, strlen(buffer));
        read_sensor_flag = 0;
    }
    if (update_display_flag) {
        // Update OLED display
        SSD1306_Clear();
        SSD1306_GotoXY(0, 0);
        SSD1306_Puts("Temperature:", &Font_11x18, 1);
        SSD1306_UpdateScreen();
        update_display_flag = 0;
    }
}
```

---

## DMA Usage
DMA (Direct Memory Access) is used to offload data transmission tasks from the CPU, making execution more efficient.

| Peripheral | Stream         | Purpose                       |
|-----------|--------------|------------------------------|
| I2C1_RX   | DMA1_Stream0 | BMP180 sensor data reception |
| I2C1_TX   | DMA1_Stream6 | SSD1306 OLED data transmission |
| USART3_TX | DMA1_Stream3 | Debug output via UART        |

### Why Use DMA?
- **Reduces CPU Load**: Data transfer happens in the background.
- **Improves Performance**: Faster sensor readout and display updates.
- **Non-blocking UART**: Ensures real-time data logging without halting execution.

---

## Current Limitations
⚠️ **Hardware Unavailability**:
- The BMP180 sensor and SSD1306 OLED are currently unavailable for physical testing.
- **Workaround**: Mock sensor data by uncommenting `#define MOCK_SENSOR 1` in `BMP180.c`.

---

## License
This project is licensed under the **STM SLA0044 License**. See [LICENSE](https://www.st.com/SLA0044) for details.

---

## Acknowledgments
- **STM32 HAL Library**
- **BMP180 Driver Reference**
- **SSD1306 Library**


