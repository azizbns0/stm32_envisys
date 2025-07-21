# 🌍 STM32 Environmental Monitoring System

[![License](https://img.shields.io/badge/License-SLA0044-blue.svg)](https://www.st.com/SLA0044)
[![Platform](https://img.shields.io/badge/Platform-STM32F4-blue)](https://www.st.com/en/microcontrollers-microprocessors/stm32f4-series.html)

An embedded real-time system for monitoring temperature, pressure, and altitude using the **BMP180 sensor** and **SSD1306 OLED** display. Built using STM32CubeIDE, STM32F4 Discovery board, and HAL libraries with DMA-enabled non-blocking I/O.

---

## 🔧 Features
- **Real-Time Sensing:** Reads temperature (°C), pressure (Pa), and altitude (m) at fixed intervals
- **OLED Visualization:** Rotates live sensor data on SSD1306 (128x64 OLED)
- **DMA + Timer-Interrupt Architecture:** Ensures low-latency, non-blocking operation
- **UART Logging:** Transmits sensor values via USART3 at 9600 baud

---

## 🔌 Hardware Overview

| Component          | Pin/Bus               | Purpose                        |
|-------------------|------------------------|--------------------------------|
| STM32F4 Discovery | -                      | Main MCU                      |
| BMP180            | I2C1 (PB6: SCL, PB7: SDA) | Environmental sensing         |
| SSD1306 OLED      | I2C1 (Shared)          | Data visualization             |
| USB-UART Adapter  | USART3 (PD8/PD9)       | Serial debug output            |

---

## 🧱 Architecture & Workflow

### ⏱ Timer (TIM2):
- Clock: 84 MHz  
- Prescaler: 8399 → 10 kHz  
- Auto-reload: 999 → **Interrupt every 100ms**

### ⌛ Task Breakdown:
| Task              | Frequency   | Trigger Flag             |
|-------------------|-------------|---------------------------|
| Sensor Read       | Every 100ms | `read_sensor_flag`       |
| Display Refresh   | Every 500ms | `update_display_flag`    |

### 💻 Main Loop Logic (Simplified):

```c
while (1) {
    if (read_sensor_flag) {
        Temperature = BMP180_GetTemp();
        Pressure = BMP180_GetPress(0);
        Altitude = BMP180_GetAlt(0);
        // Send via UART
        sprintf(buf, "T: %.2f°C P: %.2fPa A: %.2fm\n", T, P, A);
        HAL_UART_Transmit_DMA(&huart3, (uint8_t*)buf, strlen(buf));
        read_sensor_flag = 0;
    }

    if (update_display_flag) {
        SSD1306_Clear();
        SSD1306_GotoXY(0, 0);
        SSD1306_Puts("Temp:", &Font_11x18, 1);
        // update screen...
        SSD1306_UpdateScreen();
        update_display_flag = 0;
    }
}
```

---

## 🚀 DMA Configuration

| Peripheral  | DMA Stream       | Purpose                     |
|-------------|------------------|-----------------------------|
| I2C1_RX     | DMA1_Stream0     | BMP180 sensor data read     |
| I2C1_TX     | DMA1_Stream6     | OLED data display           |
| USART3_TX   | DMA1_Stream3     | UART debug transmission     |

> DMA is used to minimize CPU load and maintain real-time responsiveness for I/O-heavy operations.

---

## 🔄 Setup Instructions

### 🛠 Prerequisites:
- STM32CubeIDE  
- STM32F4 Discovery board  
- BMP180 Sensor  
- SSD1306 OLED  
- USB-UART adapter (e.g., FTDI)

### 📦 Installation:
```bash
git clone https://github.com/azizbns0/stm32_envisys.git
```

- Import to STM32CubeIDE:  
  `File → Import → Existing Projects into Workspace`

- Build and flash:  
  `Ctrl+B → Run → Debug`

---

## ⚠️ Known Limitations
- **Hardware availability issue**: BMP180 & SSD1306 not physically testable currently  
  → Enable mock data with:  
  ```c
  #define MOCK_SENSOR 1  // in BMP180.c
  ```

---

## 🧠 Learning Takeaways
- **Task scheduling with timer interrupts**
- **Non-blocking data flows using DMA**
- **Multi-peripheral I2C sharing**
- **Low-resource visualization and debug logging**

---

## 🪪 License
Licensed under STMicroelectronics [SLA0044](https://www.st.com/SLA0044)

---

## 🙏 Acknowledgments
- HAL library and STM32CubeIDE  
- BMP180 & SSD1306 open-source libraries  
- DMA tutorial examples from ST Community

---

> 🔍 *Want to collaborate or fork this system for long-term IoT deployment or remote logging projects?*  
> **Let’s connect:** [LinkedIn](https://www.linkedin.com/in/mohamed-aziz-ben-souissi-101099168/)
