# 🚀 Press Sensor Tester - Dual MCU Measurement and Control System

## 📋 1. Overview

- **Press Sensor Tester** is a resistance-based testing system designed to evaluate soft and flexible sensors under varying tensile forces. The system dynamically applies force to the sensor (from 0 to 5 kg) while measuring its resistance response over a wide range (0.4Ω to 10MΩ). This setup enables precise characterization of sensor behavior under mechanical deformation.

Key highlights of the project include:

A custom-designed 2-layer PCB with two independent STM32F103 microcontrollers:
one dedicated to system control and the other to user interface management.

Optimized real-time performance through a custom RTOS (Real-Time Operating System).

Real-time data acquisition and visualization via UART communication, with live graph display on a connected PC.

---

## ⚙️ 2. Technologies Used

- **Microcontroller**: STM32F103 (ARM Cortex-M3, 72 MHz clock) — chosen for its performance and peripheral capabilities.

- **Dual MCU Architecture**: The system uses two independent STM32F103 microcontrollers — one for real-time control and data acquisition, the other for managing the user interface.

- **Resistance Measurement**: Utilizes a constant current source method and samples voltage using a high-resolution ADS1115 ADC to compute resistance from 0.4Ω to 10MΩ.

- **Force Sensing**: Employs a load cell for tensile force measurement, digitized using the HX711 24-bit ADC.

- **Real-Time Operating System (RTOS)**: A self-developed RTOS is implemented to handle concurrent tasks with minimal latency and deterministic behavior.

- **User Interface**: **TFT display** with button-based control for settings and options.

- **UART Communication**: Data is streamed to a PC via UART, enabling live monitoring and graphing of resistance and force in real-time.

---

## ✨ 3. Key Features

- **Wide Resistance Range**: Measures sensor resistance from 0.4Ω to 10MΩ, suitable for soft, stretchable materials.

- **Tensile Force Measurement**: Accurately senses pulling force from 0 kg to 5 kg using a calibrated load cell.

- **Dual-MCU Design**: Separates real-time data processing from user interface logic for improved performance and system stability.

- **High-Precision ADCs**:

    - ADS1115 for analog voltage sampling in resistance measurement.

    - HX711 for high-resolution load cell signal acquisition.

- **Custom RTOS**: A lightweight real-time operating system ensures deterministic execution of time-sensitive tasks.

- **Real-Time Data Visualization**: Sends measurement data via UART to a PC for live plotting and logging.

- **Modular Design**: PCB and firmware structured for easy modification or adaptation to other sensor types or test setups.

- **Powered via**: adapter 12V-5A.

---

## 🛠️ 4. How to Use

### Step 1: Clone the Repository
Clone the repository to your local machine:
git clone https://github.com/nguyentriet0810/PressSensorTester
This repository contains both the firmware and PCB design files.

### Step 2: Fabricate the PCB
Navigate to the hardware/ folder and use the provided Gerber files or Altium project to manufacture the 2-layer PCB.

### Step 3: Flash the Firmware
Open the firmware project (mcu and vcu) in Keil uVision (MDK-ARM)
Connect the STM32F103C8 board via ST-Link
Build and flash the firmware vcu to the device connect to TFT
Build and flash the firmware mcu to the device control system

### Step 4: Connect to PC via UART
Use a USB-to-UART adapter (e.g., CP2102, FTDI)
Connect to the device's UART TX/RX pins
Open a serial terminal at 115200 baud
Observe live resistance readings for logging or analysis

## 🎥 5. Demo
Watch a demonstration of **Press Sensor Tester** in action: https://youtu.be/R6FCAzHkfhU

## 🧑‍💻 6. Author

**Nguyen Hoang Minh Triet** – Final Year Student at **HCM University of Technology**  
Email: [23trietminh23@gmail.com](mailto:23trietminh23@gmail.com)  
GitHub: [@nguyentriet0810](https://github.com/nguyentriet0810)  
YouTube: [Hoang Triet](https://www.youtube.com/@hoangtriet9999)

Feel free to open an issue or contact me for any questions or suggestions!




