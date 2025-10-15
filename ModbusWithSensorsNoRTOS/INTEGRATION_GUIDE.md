# ModbusWithSensorsNoRTOS - Complete Integration Documentation

## 🎯 Project Overview

**ModbusWithSensorsNoRTOS** combines 4×MQ2 gas sensors and 1×SCD30 environmental sensor into a unified Modbus RTU slave for ROBOCOMP Smash Bot Arena infrastructure monitoring.

### Key Features

- ✅ **STM32F303K8T6** @ 64MHz (optimized clock configuration)
- ✅ **4-Channel MQ2** gas sensors (PA0-PA3 via ADC1 + DMA)
- ✅ **SCD30 Environmental** sensor (I2C1 on PB6/PB7)
- ✅ **Modbus RTU Slave** (UART1 + RS485 on PA9/PA10/PA12)
- ✅ **Proven stModbus Library** (9% error rate from modbusTrying project)
- ✅ **1-Second Timer** (TIM3) for periodic sensor updates
- ✅ **No RTOS** - Pure polling/interrupt architecture

---

## 📋 Modbus Register Map

### MQ2 Gas Sensor Data (40001-40012)

| Address | Description         | Data Type | Units      | Range           |
| ------- | ------------------- | --------- | ---------- | --------------- |
| 40001   | MQ2 CH0 ADC Raw     | uint16    | ADC counts | 0-4095          |
| 40002   | MQ2 CH1 ADC Raw     | uint16    | ADC counts | 0-4095          |
| 40003   | MQ2 CH2 ADC Raw     | uint16    | ADC counts | 0-4095          |
| 40004   | MQ2 CH3 ADC Raw     | uint16    | ADC counts | 0-4095          |
| 40005   | MQ2 CH0 Voltage     | uint16    | Millivolts | 0-3300          |
| 40006   | MQ2 CH1 Voltage     | uint16    | Millivolts | 0-3300          |
| 40007   | MQ2 CH2 Voltage     | uint16    | Millivolts | 0-3300          |
| 40008   | MQ2 CH3 Voltage     | uint16    | Millivolts | 0-3300          |
| 40009   | MQ2 CH0 Digital Out | uint16    | Boolean    | 1=Gas, 0=No Gas |
| 40010   | MQ2 CH1 Digital Out | uint16    | Boolean    | 1=Gas, 0=No Gas |
| 40011   | MQ2 CH2 Digital Out | uint16    | Boolean    | 1=Gas, 0=No Gas |
| 40012   | MQ2 CH3 Digital Out | uint16    | Boolean    | 1=Gas, 0=No Gas |

### SCD30 Environmental Data (40013-40015)

| Address | Description       | Data Type | Formula            | Range                |
| ------- | ----------------- | --------- | ------------------ | -------------------- |
| 40013   | CO2 Concentration | uint16    | Direct ppm         | 0-65535 ppm          |
| 40014   | Temperature       | uint16    | (°C × 100) + 32768 | -327.68 to +327.67°C |
| 40015   | Humidity          | uint16    | % × 100            | 0.00 to 655.35%      |

### System Status (40016-40018)

| Address | Description        | Data Type | Units   | Notes                     |
| ------- | ------------------ | --------- | ------- | ------------------------- |
| 40016   | SCD30 Data Ready   | uint16    | Boolean | 1=Ready, 0=Not Ready      |
| 40017   | System Uptime      | uint16    | Seconds | Rolls over at 65535       |
| 40018   | Last Sensor Update | uint16    | Seconds | Timestamp of last reading |

### Control/Configuration (40019-40020)

| Address | Description  | Write Value | Action                      |
| ------- | ------------ | ----------- | --------------------------- |
| 40019   | System Reset | 0x1234      | Triggers NVIC_SystemReset() |
| 40020   | Force Update | 0x5678      | Immediate sensor reading    |

---

## 🔌 Hardware Configuration

### Pin Assignments

```
ADC1 (MQ2 Gas Sensors - Analog):
├── PA0 (ADC1_IN1) → MQ2 Sensor CH0 AOUT
├── PA1 (ADC1_IN2) → MQ2 Sensor CH1 AOUT
├── PA2 (ADC1_IN3) → MQ2 Sensor CH2 AOUT
└── PA3 (ADC1_IN4) → MQ2 Sensor CH3 AOUT

GPIO (MQ2 Gas Sensors - Digital):
├── PA4 → MQ2 Sensor CH0 DOUT (gas detection)
├── PA5 → MQ2 Sensor CH1 DOUT (gas detection)
├── PA6 → MQ2 Sensor CH2 DOUT (gas detection)
└── PA7 → MQ2 Sensor CH3 DOUT (gas detection)

UART1 + RS485 (Modbus RTU):
├── PA9  → UART1_TX
├── PA10 → UART1_RX
└── PA12 → RS485 DE/RE

I2C1 (SCD30 Environmental):
├── PB6 → I2C1_SCL (with 5kΩ pull-up)
└── PB7 → I2C1_SDA (with 5kΩ pull-up)

System:
└── PB3 → LED (1Hz heartbeat)
```

### Clock Configuration

- **System Clock**: 64MHz (HSI 8MHz × PLL×16 ÷ 2)
- **ADC Clock**: 32MHz (PLLCLK ÷ 2)
- **UART Clock**: 64MHz (SYSCLK)
- **I2C Clock**: 64MHz (SYSCLK)

---

## 🏗️ Software Architecture

### Core Components

#### 1. **Sensor Layer** (`sensors.h/c`)

```c
// Unified sensor interface
void Sensors_Init(void);
void Sensors_UpdateAll(void);
uint16_t Sensors_GetMQ2Value(uint8_t channel);
float Sensors_GetSCD30_CO2(void);
```

#### 2. **Modbus Layer** (`modbus*.h/c`)

- **stModbus Library**: Proven RTU implementation
- **Device Interface**: Maps sensors to registers
- **UART Callbacks**: DMA + idle line detection
- **Recovery System**: Error handling and monitoring

#### 3. **Timer System** (`main.c`)

- **TIM3**: 1-second interrupt for sensor updates
- **HAL_TIM_PeriodElapsedCallback()**: Updates all sensors
- **LED Heartbeat**: Visual system status on PB3

### Data Flow

```
┌─────────────┐    ┌──────────────┐    ┌─────────────┐
│   Sensors   │───▶│    Timer     │───▶│   Modbus    │
│ MQ2 + SCD30 │    │   1Hz TIM3   │    │ RTU Slave   │
└─────────────┘    └──────────────┘    └─────────────┘
       ▲                   │                   │
       │            ┌──────▼──────┐           │
       └────────────│ Update Loop │           │
                    │   main.c    │           │
                    └─────────────┘           │
                            ▲                 │
                            └─────────────────┘
```

---

## 🔧 Physical Wiring Guide

### MQ2 Gas Sensors (4 units)

```
Each MQ2 Sensor:
├── VCC → 5V (via SPF-02259 level converter)
├── GND → GND
├── AOUT → PA0/PA1/PA2/PA3 (via SPF-02259: 5V→3.3V)
└── DOUT → PA4/PA5/PA6/PA7 (via SPF-02259: 5V→3.3V)

SPF-02259 Level Converter #1 (Analog):
├── HV → 5V supply
├── LV → 3.3V (from STM32)
├── HV1-4 → MQ2 AOUT pins (CH0-CH3)
└── LV1-4 → STM32 PA0-PA3

SPF-02259 Level Converter #2 (Digital):
├── HV → 5V supply
├── LV → 3.3V (from STM32)
├── HV1-4 → MQ2 DOUT pins (CH0-CH3)
└── LV1-4 → STM32 PA4-PA7
```

### SCD30 Environmental Sensor

```
SCD30 Connections:
├── VDD → 3.3V (STM32 supply)
├── GND → GND
├── SCL → PB6 (with 5kΩ pull-up to 3.3V)
├── SDA → PB7 (with 5kΩ pull-up to 3.3V)
└── RDY → Not connected

Pull-up Resistors:
├── R1: 5kΩ (PB6 to 3.3V) - Use 2×10kΩ in parallel
└── R2: 5kΩ (PB7 to 3.3V) - Use 2×10kΩ in parallel
```

### RS485 Interface

```
RS485 Module:
├── VCC → 3.3V
├── GND → GND
├── DI → PA9 (UART1_TX)
├── RO → PA10 (UART1_RX)
├── DE → PA12 (GPIO output)
├── RE → PA12 (GPIO output)
├── A → RS485 A+ (to network)
└── B → RS485 B- (to network)
```

---

## 📊 Performance Specifications

### Sensor Update Rate

- **MQ2 Sensors**: 4 channels @ 1Hz (181.5 cycles sampling)
- **SCD30 Sensor**: Data ready polling @ 1Hz
- **Modbus Registers**: Updated every 1 second via TIM3

### Modbus Communication

- **Baud Rate**: 9600 bps (standard Modbus RTU rate, configurable in CubeMX)
- **Data Format**: 8N1 (8 data, no parity, 1 stop)
- **Slave Address**: 0x01 (configurable in modbus_init.c)
- **Error Rate**: ~9% (inherited from proven modbusTrying implementation)

### Memory Usage

- **Flash**: ~32KB (stModbus + sensor drivers + HAL)
- **RAM**: ~4KB (buffers + sensor data + stack)
- **Registers**: 20×16-bit Modbus holding registers

---

## 🚀 Quick Start Guide

### 1. Build and Flash

```bash
# In STM32CubeIDE or command line:
make clean && make all
st-flash write build/ModbusWithSensorsNoRTOS.bin 0x8000000
```

### 2. Hardware Setup

1. Connect MQ2 sensors to PA0-PA3 via SPF-02259 level converters
2. Connect SCD30 to PB6/PB7 with 5kΩ pull-ups
3. Connect RS485 module to PA9/PA10/PA12
4. Power system with 5V supply (for MQ2) and 3.3V for STM32

### 3. Modbus Testing

```bash
# Using ModbusPoll or similar tool:
# Device: COM port at 9600 bps
# Slave ID: 1
# Function: 03 (Read Holding Registers)
# Address: 40001, Count: 20

# Expected values:
# 40001-40004: MQ2 ADC values (0-4095)
# 40005-40008: MQ2 voltages (mV)
# 40009: MQ2 digital status (bit 0=CH0, bit 1=CH1, bit 2=CH2, bit 3=CH3)
# 40010: CO2 concentration (ppm)
# 40011: Temperature (°C×100+32768)
# 40012: Humidity (RH%×100)
```

### System Verification

- **LED Heartbeat**: PB3 should blink at 1Hz
- **Modbus Response**: All 20 registers should be readable
- **Sensor Data**: Values should update every second
- **Communication**: <10% error rate expected

### MQ2 Digital Status Decoding (Register 40009)

```c
// Read register 40009 to get packed digital status
uint16_t digital_status = read_register(40009);

// Extract individual channel status
bool ch0_gas = (digital_status & 0x01) ? true : false;  // Bit 0
bool ch1_gas = (digital_status & 0x02) ? true : false;  // Bit 1
bool ch2_gas = (digital_status & 0x04) ? true : false;  // Bit 2
bool ch3_gas = (digital_status & 0x08) ? true : false;  // Bit 3

// Example: digital_status = 0x0005 means CH0 and CH2 detect gas
```

---

## 🛠️ Troubleshooting

### Common Issues

#### 1. **No Modbus Communication**

- Check RS485 wiring (A+/B- polarity)
- Verify baud rate (9600 bps - standard Modbus RTU)
- Ensure slave address matches (0x01)
- Check termination resistors on RS485 bus

#### 2. **SCD30 Not Responding**

- Verify I2C pull-up resistors (5kΩ)
- Check 3.3V power supply stability
- Ensure proper SDA/SCL connections (PB6/PB7)
- Allow 1-second startup delay

#### 3. **MQ2 Values Stuck at 0**

- Check 5V supply for MQ2 sensors
- Verify SPF-02259 level converter wiring
- Ensure ADC calibration completed
- Check PA0-PA3 pin configuration

#### 4. **System Not Responsive**

- Check 64MHz clock configuration
- Verify TIM3 1-second timer operation
- Monitor LED heartbeat on PB3
- Check for infinite loops in sensor code

### Debug Features

```c
// Enable debug output in modbus_device.c:
void Modbus_Device_DebugArray(void)
{
    printf("=== Modbus Registers ===\n");
    for (int i = 0; i < 20; i++)
    {
        printf("40%03d: %5d (0x%04X)\n", i+1, device_registers[i], device_registers[i]);
    }
}
```

---

## 📈 Future Enhancements

### Planned Features

1. **Adaptive Sampling**: Dynamic sensor update rates based on change detection
2. **Data Logging**: Local storage with timestamp and statistics
3. **Calibration System**: Field calibration for MQ2 gas response curves
4. **Alarm Thresholds**: Configurable limits with digital output control
5. **Network Discovery**: Auto-configuration and device identification

### Expansion Possibilities

- Additional sensor types via SPI/I2C
- Local data processing and trend analysis
- Wireless communication (LoRa, WiFi, etc.)
- Integration with SCADA systems
- Real-time alarm notification system

---

## 📚 References

1. **stModbus Library**: [GitHub Repository](https://github.com/urands/stModbus)
2. **STM32F303K8**: [Datasheet](https://www.st.com/resource/en/datasheet/stm32f303k8.pdf)
3. **Modbus RTU Specification**: [Archived PDF](https://web.archive.org/web/20250825211935/https://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf)
4. **Modbus Serial Line**: [Implementation Guide](https://web.archive.org/web/20250910152913if_/https://www.modbus.org/docs/Modbus_over_serial_line_V1_02.pdf)
5. **SCD30 Datasheet**: [Sensirion Documentation](https://sensirion.com/products/catalog/SCD30/)
6. **MQ2 Gas Sensor**: [Datasheet and Application Notes](https://www.pololu.com/file/0J309/MQ2.pdf)

---

**Project Status**: ✅ **Integration Phase 2 Complete**  
**Next Phase**: Testing and validation with real hardware setup

Last Updated: October 15, 2025
