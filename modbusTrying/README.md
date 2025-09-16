# STM32F303K8 Modbus RTU Slave Example

This project demonstrates a working Modbus RTU slave implementation on the STM32F303K8 microcontroller using the stModbus library. It is designed as a reference for building robust Modbus RTU devices on STM32, with a focus on clarity, portability, and real-world hardware integration.

## Features

- **Modbus RTU slave** on USART1 (RS485, 9600 baud, 8N1)
- **DMA-based UART** for efficient communication
- **20 holding registers** (40001–40020) with easy mapping
- **Callback-based register access** for flexible data handling
- **Ready for real sensor integration**
- **No dependency on external ModbusRep folder**
- **Tested with ModbusPoll**

## Project Structure

```
Core/
  Inc/
    main.h
    modbus.h
    modbus_conf.h
    mbdevice.h
    mbutils.h
    modbus_init.h
    modbus_device.h
    uart_callbacks.h
    modbus_test.h
  Src/
    main.c
    modbus.c
    mbutils.c
    modbus_init.c
    modbus_device.c
    uart_callbacks.c
    modbus_test.c
    stm32f3xx_it.c
    stm32f3xx_hal_msp.c
Drivers/
  STM32F3xx_HAL_Driver/
  CMSIS/
```

## Hardware Setup

- **MCU**: STM32F303K8T6
- **RS485**: Hardware transceiver, DE/RE pin on PA12
- **UART**: USART1 (PA9 TX, PA10 RX)
- **Termination**: 120Ω at each end of RS485 bus

## Modbus Register Map

| Register | Address | Default Value | Description          |
| -------- | ------- | ------------- | -------------------- |
| 40001    | 0       | 20            | Sensor 1/Test value  |
| 40002    | 1       | 19            | Sensor 2/Test value  |
| 40003    | 2       | 18            | Sensor 3/Test value  |
| 40004    | 3       | 17            | Sensor 4/Test value  |
| 40005    | 4       | 16            | Sensor 5/Test value  |
| 40006    | 5       | 15            | Sensor 6/Test value  |
| 40007    | 6       | 14            | Sensor 7/Test value  |
| 40008    | 7       | 13            | Sensor 8/Test value  |
| 40009    | 8       | 12            | Sensor 9/Test value  |
| 40010    | 9       | 11            | Sensor 10/Test value |
| 40011    | 10      | 10            | Sensor 11/Test value |
| 40012    | 11      | 9             | Sensor 12/Test value |
| 40013    | 12      | 8             | Sensor 13/Test value |
| 40014    | 13      | 7             | Sensor 14/Test value |
| 40015    | 14      | 6             | Sensor 15/Test value |
| 40016    | 15      | 5             | Sensor 16/Test value |
| 40017    | 16      | 4             | Sensor 17/Test value |
| 40018    | 17      | 3             | Sensor 18/Test value |
| 40019    | 18      | 2             | Sensor 19/Test value |
| 40020    | 19      | 1             | Sensor 20/Test value |

## Quick Start

1. **Build and flash** the firmware to your STM32F303K8 board.
2. **Connect RS485**: STM32 to USB-RS485 converter, ensure proper DE/RE control and termination.
3. **Open ModbusPoll** (or similar):
   - Slave ID: 1
   - Function: 03 (Read Holding Registers)
   - Address: 40001
   - Quantity: 20
   - Baud: 9600, 8N1, No parity
4. **Read registers**: You should see test values (default: 20, 19, ..., 1)

## Customization

- **Replace test values** in `Modbus_Device_UpdateSensors()` with real sensor reads.
- **Change register mapping** in `modbus_device.c`.
- **Adjust UART/RS485 config** in CubeMX and regenerate code if needed.

## Troubleshooting

- **Timeouts/No response**: Check wiring, DE/RE pin, and baud rate.
- **Wrong values**: Confirm register mapping and ModbusPoll address offset.
- **CRC errors**: Check UART settings and cable quality.

## Notes

- This project is self-contained and does not require the original ModbusRep folder.
- All Modbus protocol logic is in `Core/Src/modbus.c` and `Core/Src/mbutils.c`.
- The code is ready for extension with FreeRTOS or real sensor drivers.

---

**Author:** Your Name / Team  
**License:** MIT  
**Last updated:** September 2025
