# ModbusWithSensorsNoRTOS Project

Combined 4×MQ2 gas sensors + SCD30 environmental sensor with Modbus RTU slave communication.

## 🎯 Project Goals

- **Multi-sensor integration**: 4× MQ2 gas sensors + 1× SCD30 environmental sensor
- **Modbus RTU communication**: RS485 interface for PLC integration
- **No RTOS**: Bare metal implementation for maximum reliability
- **Arena infrastructure**: Integrated part of ROBOCOMP Smash Bot Arena system

## ⚙️ CubeMX Configuration Progress

### ✅ UART1 Configuration (Modbus RTU) - COMPLETED

- **Mode**: Asynchronous
- **Hardware Flow Control**: ✅ **Enabled** (auto-configured PA12 as DE pin)
- **Pin Assignment**:
  - PA9 → USART1_TX
  - PA10 → USART1_RX
  - PA12 → USART1_DE (automatic via Hardware Flow Control)
- **Parameters**:
  - Baud Rate: 9600 Bit/s
  - Word Length: 8 bits
  - Parity: None
  - Stop Bits: 1
- **DMA Configuration**:
  - DMA1 Channel 4: USART1_TX (with global interrupt)
  - DMA1 Channel 5: USART1_RX (with global interrupt)
- **Interrupts**:
  - ✅ USART1 global interrupt
  - ✅ USART1 wake-up interrupt
  - ✅ DMA1 Channel 4 global interrupt
  - ✅ DMA1 Channel 5 global interrupt

### 🔄 Remaining CubeMX Configuration

#### 1. ADC1 Configuration (MQ2 Sensors) - IN PROGRESS

- [x] **Enable ADC1**
- [x] **Add Channels**: IN1 (PA0), IN2 (PA1), IN3 (PA2), IN4 (PA3) ✅
- [ ] **Resolution**: 12 bits
- [ ] **Data Alignment**: Right aligned
- [ ] **Scan Conversion Mode**: Enable
- [ ] **Continuous Conversion**: Disable
- [ ] **Number of Conversions**: 4
- [ ] **DMA**: Enable DMA1 Channel 1 (Circular mode, Half Word)
- [ ] **Enable ADC1 global interrupt**

#### 2. GPIO Configuration (MQ2 Digital Inputs)

- [ ] **PA4**: GPIO_Input with Pull-up (MQ2 #1 Digital)
- [ ] **PA5**: GPIO_Input with Pull-up (MQ2 #2 Digital) ✅ (no conflict)
- [ ] **PA6**: GPIO_Input with Pull-up (MQ2 #3 Digital)
- [ ] **PA7**: GPIO_Input with Pull-up (MQ2 #4 Digital)
- [ ] **PB3**: Built-in LED (LD3) - available for status indication

#### 3. I2C1 Configuration (SCD30 Sensor) - ✅ COMPLETED

- [x] **Enable I2C1**
- [x] **Pin Assignment**:
  - PA14 → I2C1_SDA ✅ (CubeMX auto-assigned)
  - PA15 → I2C1_SCL ✅ (CubeMX auto-assigned)
- [x] **Speed Mode**: Standard Mode (100 kHz)
- [x] **Own Address**: 0x00
- [x] **Addressing Mode**: 7-bit
- [x] **General Call**: Disable
- [x] **DMA**: ❌ Disabled (use polling for simplicity)
- [x] **Interrupts**:
  - [x] ✅ I2C1 error interrupt (enabled)
  - [x] ❌ I2C1 event interrupt (skipped)
  - [x] ❌ I2C1 wake-up interrupt (skipped)

#### 4. Timer Configuration (System Timing) - ✅ COMPLETED

- [x] **Enable TIM3** for general timing (TIM2 has conflicts)
- [x] **Prescaler**: 63999 (for 1kHz tick at 64MHz)
- [x] **Counter Period**: 999 (for 1-second intervals)
- [x] **Auto-reload preload**: Enable
- [x] **Enable TIM3 global interrupt**

#### 5. Clock Configuration - ✅ COMPLETED

- [x] **System Clock**: 64 MHz (HSI + PLL)
- [x] **PLL**: HSI × 8 = 64 MHz
- [x] **HCLK**: 64 MHz
- [x] **APB1**: 32 MHz (PCLK1)
- [x] **APB2**: 64 MHz (PCLK2)
- [x] **ADC Clock**: 32 MHz
- [x] **USART1 Clock**: SYSCLK (64 MHz)
- [x] **I2C1 Clock**: SYSCLK (64 MHz)

#### 6. Code Generation Settings

- [ ] **Toolchain**: STM32CubeIDE
- [ ] **Code Generator**: Copy only necessary files
- [ ] **Generated Function Calls**: HAL
- [ ] **Keep User Code**: Enable

## 📋 Pin Assignment Summary

```
Modbus RTU (RS485):
├── PA9  → USART1_TX
├── PA10 → USART1_RX
└── PA12 → USART1_DE (Hardware Flow Control)

MQ2 Sensors (4×):
├── PA0 → ADC1_IN1  (MQ2 #1 Analog)
├── PA1 → ADC1_IN2  (MQ2 #2 Analog)
├── PA2 → ADC1_IN3  (MQ2 #3 Analog)
├── PA3 → ADC1_IN4  (MQ2 #4 Analog)
├── PA4 → GPIO_IN   (MQ2 #1 Digital)
├── PA5 → GPIO_IN   (MQ2 #2 Digital)
├── PA6 → GPIO_IN   (MQ2 #3 Digital)
└── PA7 → GPIO_IN   (MQ2 #4 Digital)

SCD30 Environmental:
├── PB6 → I2C1_SCL ✅ (Fixed debugger conflict)
└── PB7 → I2C1_SDA ✅ (Fixed debugger conflict)

System:
└── PB3  → Built-in LED (LD3) - Status indication
```

## 🚨 Critical Reminders for CubeMX

### Don't Forget:

1. **ADC DMA**: Must be Circular mode, Half Word for continuous 4-channel scanning
2. **I2C Pull-ups**: ✅ **Required** - Use 2× 10kΩ resistors in parallel (5kΩ effective) from 3V3 to SCL/SDA
3. **Clock Tree**: Ensure 64 MHz system clock for optimal performance
4. **Interrupt Priorities**: Keep UART higher priority than ADC/Timer for Modbus reliability
5. **GPIO Speed**: Set UART pins to High Speed for reliable RS485 communication

### Next Steps After Code Generation:

1. Copy optimized Modbus implementation from `modbusTrying/` project
2. Integrate MQ2 sensor reading from `MQ2_1/` project
3. Add SCD30 I2C communication from `SCD30/` project
4. Implement combined sensor register mapping
5. Test Modbus communication with all sensors active

## 🔌 Physical Connections with SPF-02259 Level Converters

### **Power Distribution:**

```
5V External Supply → MQ2 sensors (×4) + SPF-02259 HV pins
STM32 3V3 pin → SCD30 + SPF-02259 LV pins
Common Ground → All components
```

### **MQ2 Gas Sensors via Level Converters:**

```
MQ2 Sensor #1:                    MQ2 Sensor #2:
├── VCC → 5V External Supply      ├── VCC → 5V External Supply
├── GND → Common Ground           ├── GND → Common Ground
├── A0  → SPF-02259 #2 HV1        ├── A0  → SPF-02259 #2 HV2
└── D0  → SPF-02259 #1 HV1        └── D0  → SPF-02259 #1 HV2

MQ2 Sensor #3:                    MQ2 Sensor #4:
├── VCC → 5V External Supply      ├── VCC → 5V External Supply
├── GND → Common Ground           ├── GND → Common Ground
├── A0  → SPF-02259 #2 HV3        ├── A0  → SPF-02259 #2 HV4
└── D0  → SPF-02259 #1 HV3        └── D0  → SPF-02259 #1 HV4
```

### **SPF-02259 Level Converter Connections:**

```
SPF-02259 #1 (Digital Signals):   SPF-02259 #2 (Analog Signals):
├── HV  → 5V External Supply       ├── HV  → 5V External Supply
├── LV  → STM32 3V3 pin           ├── LV  → STM32 3V3 pin
├── GND → Common Ground           ├── GND → Common Ground
├── LV1 → PA4 (MQ2 #1 Digital)    ├── LV1 → PA0 (MQ2 #1 Analog)
├── LV2 → PA5 (MQ2 #2 Digital)    ├── LV2 → PA1 (MQ2 #2 Analog)
├── LV3 → PA6 (MQ2 #3 Digital)    ├── LV3 → PA2 (MQ2 #3 Analog)
└── LV4 → PA7 (MQ2 #4 Digital)    └── LV4 → PA3 (MQ2 #4 Analog)
```

### **Other Connections:**

```
SCD30 Environmental Sensor:       RS485 Modbus Interface:
├── VCC → STM32 3V3 pin           ├── VCC → STM32 3V3 pin
├── GND → Common Ground           ├── GND → Common Ground
├── SCL → PB6 (I2C1_SCL)          ├── DI  → PA9 (USART1_TX)
└── SDA → PB7 (I2C1_SDA)          ├── RO  → PA10 (USART1_RX)
                                  ├── DE  → PA12 (RS485 Control)
                                  ├── A+  → Master A+
                                  └── B-  → Master B-

I2C Pull-up Resistors (Required):
├─��� SCL Pull-up: 3V3 → 5kΩ → PB6 (SCL)
└── SDA Pull-up: 3V3 → 5kΩ → PB7 (SDA)
   Note: Use 2× 10kΩ resistors in parallel = 5kΩ effective
```

### **✅ Advantages of This Setup:**

- **Optimal MQ2 performance**: 5V operation for better heating/accuracy
- **Perfect voltage matching**: Automatic 5V→3.3V conversion
- **STM32 protection**: All signals guaranteed safe 3.3V levels
- **Clean analog signals**: No voltage divider noise
- **Expandable**: Extra SPF-02259 channels available for future sensors

---

## 🚀 Code Generation Analysis - EXCELLENT!

### ✅ **Generated Code Verification:**

- **All peripherals properly configured** (ADC1, I2C1, UART1, TIM3, GPIO) ✅
- **64MHz system clock optimized** (HSI + PLL × 16 configuration) ✅
- **DMA channels correctly assigned** (ADC, UART TX/RX) ✅
- **Interrupt handlers present** (ADC, UART, I2C error) ✅
- **Pin assignments match design** (PA0-PA3 ADC, PA4-PA7 GPIO) ✅

### ✅ **All Systems Ready:**

- **TIM3 interrupt**: `TIM3_IRQHandler` now properly generated ✅

### 🎯 **Ready for Integration Phase:**

1. **Copy Modbus implementation** from `modbusTrying/` project (proven reliable)
2. **Add sensor reading functions** from `MQ2_1/` and `SCD30/` projects
3. **Implement system integration** (sensors → Modbus registers mapping)
4. **Test with SPF-02259 level converters** and physical sensors

---

**Status**: CubeMX ✅ Complete | Code Generation ✅ Perfect | TIM3 Interrupt ✅ Fixed | Integration 🚀 Ready!
