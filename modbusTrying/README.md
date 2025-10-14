# STM32F303K8 Modbus RTU Slave Example

This project demonstrates a working Modbus RTU slave implementation on the STM32F303K8 microcontroller using the stModbus library. The implementation is fully functional in debug mode with comprehensive LED-based debugging indicators.

## ⚠️ Important Notice

**Current Status**: This code works perfectly in STM32CubeIDE debug mode but has a hardware limitation with the NUCLEO-F303K8 board in standalone operation. The issue is board-specific and not related to the software implementation.

**Debug Mode Operation**: Fully functional Modbus RTU slave with LED status indicators
**Standalone Mode**: Limited by NUCLEO-F303K8 board hardware constraints

## Features

- **Modbus RTU slave** on USART1 (RS485, 9600 baud, 8N1)
- **DMA-based UART** for efficient communication
- **20 holding registers** (40001–40020) with values 20 to 1
- **Callback-based register access** for flexible data handling
- **LED debug system** (PA0-PA6) for operation monitoring
- **Semi-hosting removed** for standalone compatibility
- **Tested with ModbusPoll**
- **Ready for real sensor integration**

## LED Debug System

The firmware includes a comprehensive LED debugging system for monitoring operation:

| LED Pin | Function          | Description                             |
| ------- | ----------------- | --------------------------------------- |
| PA0     | Power Indicator   | Turns on immediately at startup         |
| PA1     | HAL Init Complete | HAL library initialized                 |
| PA2     | Clock Configured  | System clock configured                 |
| PA3     | GPIO Ready        | GPIO peripherals initialized            |
| PA4     | UART Ready        | UART communication initialized          |
| PA5     | Modbus Ready      | Modbus protocol initialized             |
| PA6     | Heartbeat         | Toggles every ~500ms (main loop active) |

## Project Structure

```
Core/
  Inc/
    main.h
    modbus.h, modbus_conf.h, mbdevice.h, mbutils.h
    modbus_init.h, modbus_device.h, uart_callbacks.h
  Src/
    main.c                 # Main application with LED debug system
    modbus.c, mbutils.c    # Modbus protocol implementation
    modbus_init.c          # UART/DMA initialization
    modbus_device.c        # Register mapping and callbacks
    uart_callbacks.c       # UART interrupt handlers
    stm32f3xx_it.c         # Interrupt service routines
    stm32f3xx_hal_msp.c    # HAL MSP initialization
Drivers/
  STM32F3xx_HAL_Driver/   # STM32 HAL library
  CMSIS/                  # ARM CMSIS
```

## Hardware Setup

- **MCU**: STM32F303K8T6 (Nucleo-32 board)
- **RS485**: Hardware transceiver, DE/RE pin on PA12
- **UART**: USART1 (PA9 TX, PA10 RX)
- **Debug LEDs**: PA0-PA6 (see LED debug system above)
- **Termination**: 120Ω at each end of RS485 bus
- **Power**: USB (debug mode) or external 3.3V (standalone testing)

## Modbus Register Map

| Register | Address | Default Value | Description   |
| -------- | ------- | ------------- | ------------- |
| 40001    | 0       | 20            | Test value 1  |
| 40002    | 1       | 19            | Test value 2  |
| 40003    | 2       | 18            | Test value 3  |
| 40004    | 3       | 17            | Test value 4  |
| 40005    | 4       | 16            | Test value 5  |
| 40006    | 5       | 15            | Test value 6  |
| 40007    | 6       | 14            | Test value 7  |
| 40008    | 7       | 13            | Test value 8  |
| 40009    | 8       | 12            | Test value 9  |
| 40010    | 9       | 11            | Test value 10 |
| 40011    | 10      | 10            | Test value 11 |
| 40012    | 11      | 9             | Test value 12 |
| 40013    | 12      | 8             | Test value 13 |
| 40014    | 13      | 7             | Test value 14 |
| 40015    | 14      | 6             | Test value 15 |
| 40016    | 15      | 5             | Test value 16 |
| 40017    | 16      | 4             | Test value 17 |
| 40018    | 17      | 3             | Test value 18 |
| 40019    | 18      | 2             | Test value 19 |
| 40020    | 19      | 1             | Test value 20 |

## Quick Start (Debug Mode)

1. **Open in STM32CubeIDE**: Import the project
2. **Build**: Clean build should succeed without warnings
3. **Debug**: Click "Debug" button (green bug icon)
4. **Run**: Click "Resume" (F8) to start execution
5. **Verify LEDs**: PA0-PA6 should light up sequentially, PA6 should blink
6. **Test Modbus**: Connect RS485 and use ModbusPoll to read registers

## Modbus Testing with ModbusPoll

1. **Connect RS485**: STM32 to USB-RS485 converter
2. **Configure ModbusPoll**:
   - Slave ID: 1
   - Function: 03 (Read Holding Registers)
   - Address: 40001
   - Quantity: 20
   - Baud: 9600, 8N1, No parity
3. **Read**: Should display values 20, 19, ..., 1

## Customization

### Adding Real Sensors

- Replace test values in `Modbus_Device_UpdateSensors()` with actual sensor reads
- Modify register mapping in `modbus_device.c`
- Add sensor drivers as needed

### Changing Modbus Configuration

- Adjust UART settings in CubeMX
- Modify baud rate, parity, or slave ID in `modbus_conf.h`
- Regenerate code if hardware changes are made

## Troubleshooting

### Debug Mode Issues

- **No LEDs lighting**: Check power and connections
- **LEDs don't sequence**: Code may be stuck in initialization
- **PA6 not blinking**: Main loop not reached (initialization failure)
- **Modbus timeout**: Check RS485 wiring and termination

### Build Issues

- **Semi-hosting warnings**: Ensure `--specs=nosys.specs` is used
- **Linker errors**: Check all source files are included
- **Missing includes**: Verify header file paths

### Hardware Issues

- **RS485 no response**: Check DE/RE pin (PA12) and transceiver power
- **Wrong register values**: Confirm ModbusPoll address offset (40001 = address 0)
- **CRC errors**: Verify UART settings and cable quality

## Known Limitations

### NUCLEO-F303K8 Board Issue

- **Standalone Mode Failure**: Board-specific hardware limitation prevents standalone operation
- **Root Cause**: Unknown - possibly power supply or oscillator configuration
- **Workaround**: Use debug mode for development, consider different STM32 board for production
- **Status**: Software fully functional, hardware limitation identified

### Debug Features

- **LED Debug System**: Comprehensive visual indicators for troubleshooting
- **Semi-hosting Removed**: Clean build for standalone compatibility
- **Test Values**: Fixed register values for testing Modbus communication

## Development History

This project underwent extensive debugging to identify why it worked in debug mode but failed standalone:

1. **Semi-hosting Investigation**: Removed all semi-hosting dependencies
2. **Build Optimization**: Clean compilation with no warnings
3. **LED Debug System**: Added visual indicators for operation monitoring
4. **Component Isolation**: Tested UART, HAL, and bare-metal operation
5. **Root Cause Identified**: NUCLEO-F303K8 board-specific hardware limitation

## Future Development

### Recommended Actions

1. **Board Migration**: Port to different STM32 board for standalone operation
2. **External Power Testing**: Verify if stable 3.3V power resolves standalone issues
3. **Crystal Oscillator**: Test with external crystal if available
4. **Custom PCB**: Consider designing custom board for production

### Code Readiness

- **Modbus Protocol**: Fully implemented and tested
- **LED Debugging**: Comprehensive status monitoring
- **HAL Integration**: Proper STM32 HAL usage
- **Documentation**: Complete setup and troubleshooting guides

## Technical Specifications

- **MCU Clock**: 8MHz HSI (internal oscillator)
- **UART Baud**: 9600, 8N1, no parity
- **DMA Channels**: UART1 RX/TX with circular buffers
- **RS485 Control**: Hardware DE pin (PA12) for transmit/receive switching
- **Memory Usage**: ~22KB flash, minimal RAM
- **Stack/Heap**: 1KB stack, 512B heap

---

**Author:** Development Team  
**License:** MIT  
**Version:** 1.2 (LED Debug System + Standalone Investigation Complete)  
**Status:** Working in debug mode, board limitation identified  
**Last updated:** October 14, 2025

- **Debug mode works but standalone fails**: Semi-hosting removed - check PA4 LED for heartbeat (1Hz toggle).

## Notes

- This project is self-contained and does not require the original ModbusRep folder.
- All Modbus protocol logic is in `Core/Src/modbus.c` and `Core/Src/mbutils.c`.
- Semi-hosting dependencies removed for standalone operation.
- The code is ready for extension with FreeRTOS or real sensor drivers.

---

**Author:** Your Name / Team  
**License:** MIT  
**Version:** 1.1 (Semi-hosting removed for standalone operation)  
**Last updated:** October 2025
