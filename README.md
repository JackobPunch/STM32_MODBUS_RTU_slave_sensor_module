# STM32 Sensor Module with Modbus RTU

A comprehensive sensor monitoring system built on STM32F303K8 microcontroller for robotic applications, specifically designed for integration with PLC systems in competitive robotics environments like the [ROBOCOMP Festival](https://www.robocomp.info/).

## 🎯 Project Overview

This repository contains multiple STM32 projects demonstrating the evolution from basic LED blinking to a complete multi-sensor Modbus RTU slave system. The final goal is to create a reliable sensor module for the **Smash Bot Arena** robotic competition, providing real-time environmental and gas detection data to master PLC systems.

### 🏆 Target Application

- **Event**: [ROBOCOMP Robotics Festival](https://www.robocomp.info/) - Poland's premier robotics competition
- **Use Case**: Smash Bot Arena environmental monitoring
- **Integration**: RS485 Modbus RTU communication with PLC master systems
- **Sensors**: Multi-gas detection (MQ2) + environmental monitoring (CO₂, temperature, humidity)

## 📁 Repository Structure

### Core Projects

| Project         | Status                  | Description                 | Purpose                                  |
| --------------- | ----------------------- | --------------------------- | ---------------------------------------- |
| `ledBlinking/`  | ✅ Complete             | Basic GPIO and HAL setup    | Learning foundation, hardware validation |
| `modbusTrying/` | ✅ **Production Ready** | Standalone Modbus RTU slave | **Main implementation** - 9% error rate  |
| `MQ2_1/`        | ✅ Complete             | Single MQ2 gas sensor       | ADC-based gas detection proof-of-concept |
| `SCD30/`        | ✅ Complete             | SCD30 environmental sensor  | I2C-based CO₂/temp/humidity monitoring   |
| `ModbusRTOS/`   | ⚠️ Experimental         | FreeRTOS + Modbus           | Future expansion (not currently needed)  |

### 🎯 Next Development

- `ModbusWithSensorsNoRTOS/` - **[In Planning]** Combined 4×MQ2 + SCD30 + Modbus system

## 🔧 Technical Specifications

### Hardware Platform

- **MCU**: STM32F303K8T6 (Cortex-M4, 64 MHz)
- **Board**: NUCLEO-F303K8
- **Flash**: 64 KB | **RAM**: 16 KB
- **Communication**: RS485 Modbus RTU at 9600 baud

### Sensor Capabilities

- **Gas Detection**: 4× MQ2 sensors (analog + digital threshold)
- **Environmental**: SCD30 (CO₂, temperature, humidity via I2C)
- **Update Rate**: 1-second sensor polling, 1000ms Modbus scan cycle
- **Reliability**: <10% communication error rate (production tested)

## 🚀 Quick Start

### 1. Hardware Setup

```
STM32F303K8 Pin Assignments:
├── PA9/PA10/PA12  → RS485 Modbus (UART1 + DE/RE)
├── PA0,PA3,PA6,PB0 → MQ2 Analog inputs (ADC1)
├── PA1,PA4,PA7,PB1 → MQ2 Digital inputs (GPIO)
├── PA15/PB3       → SCD30 I2C (SDA/SCL)
└── PA5            → Status LED
```

### 2. Development Environment

- **IDE**: STM32CubeIDE 1.8+
- **HAL**: STM32F3xx HAL Driver
- **Debugger**: ST-LINK v2.1 (on-board)
- **Build**: ARM GCC with `--specs=nosys.specs`

### 3. Modbus Implementation

Based on [stModbus library](https://github.com/urands/stModbus) with custom optimizations:

```c
// Modbus Register Map (Holding Registers)
0x0001-0x0004: MQ2 Analog Values [0-4095]
0x0005-0x0008: MQ2 Digital States [0/1]
0x0009: CO₂ Level [ppm]
0x000A: Temperature [°C × 10]
0x000B: Humidity [% × 10]
0x000C: System Status Flags
```

## 📊 Project Evolution

### Phase 1: Foundation ✅

- **`ledBlinking/`**: HAL initialization and GPIO control
- **Learning**: STM32CubeIDE workflow, debugging, flashing

### Phase 2: Communication ✅

- **`modbusTrying/`**: Standalone Modbus RTU slave implementation
- **Achievements**: 9% error rate, recovery system, production-ready reliability
- **Key Features**: Advanced error recovery, software watchdog, optimized timing

### Phase 3: Sensor Integration ✅

- **`MQ2_1/`**: Gas sensor with ADC polling (24μs conversion time)
- **`SCD30/`**: Environmental sensor with I2C + CRC validation
- **Performance**: <10ms total sensor reading time

### Phase 4: System Integration 🔄

- **`ModbusWithSensorsNoRTOS/`**: Combined multi-sensor Modbus system
- **Target**: Production sensor module for robotic competitions

## 🛡️ Reliability Features

### Modbus Recovery System

- **Activity Monitoring**: Tracks communication health
- **Automatic Recovery**: Resets on timeout/error conditions
- **Error Tracking**: <10% error rate in production testing
- **Watchdog Protection**: Software watchdog prevents system hangs

### Sensor Validation

- **MQ2 Sensors**: Analog + digital threshold validation
- **SCD30**: I2C CRC validation and data-ready polling
- **Fault Tolerance**: Graceful degradation on sensor failures

## 📈 Performance Metrics

| Metric                 | Value | Notes                             |
| ---------------------- | ----- | --------------------------------- |
| **Modbus Error Rate**  | <9%   | Down from 40% after optimization  |
| **Sensor Update Rate** | 1 Hz  | Adequate for robotic applications |
| **ADC Conversion**     | 24μs  | 4-channel sequential conversion   |
| **I2C Transaction**    | <10ms | Including CRC validation          |
| **Recovery Time**      | <2s   | Automatic error recovery          |

## 🔗 References & Acknowledgments

### Key Resources

- **Modbus Implementation**: [stModbus Library](https://github.com/urands/stModbus) by urands
- **Target Competition**: [ROBOCOMP Robotics Festival](https://www.robocomp.info/)
- **Hardware**: STM32 NUCLEO-F303K8 Development Board
- **Protocol**: Modbus RTU over RS485 (Industrial Standard)

### Development Notes

- **FreeRTOS Consideration**: `ModbusRTOS/` project exists for future expansion but is not required for current functionality
- **Error Recovery**: Aggressive recovery system necessary for industrial reliability in competition environment
- **Sensor Integration**: Multi-sensor timing carefully optimized to prevent Modbus communication interference

## 📝 License & Usage

This project is developed for educational and competitive robotics purposes. When using or adapting this code:

1. **Hardware Safety**: Ensure proper power supply sizing for MQ2 sensor heating elements
2. **Modbus Network**: Follow RS485 termination and grounding best practices
3. **Sensor Calibration**: MQ2 sensors require warm-up period and environmental calibration
4. **Competition Rules**: Verify sensor types and communication protocols are permitted in your specific competition

---

**Developed for ROBOCOMP Smash Bot Arena** | **STM32F303K8** | **Modbus RTU** | **Production Ready**
