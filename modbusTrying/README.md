# STM32F303K8 Modbus RTU Slave Implementation

This project implements a Modbus RTU slave using the stModbus library on STM32F303K8 microcontroller.

## Project Structure

```
Core/
├── Inc/
│   ├── main.h
│   ├── modbus.h
│   ├── modbus_conf.h
│   ├── mbdevice.h
│   ├── mbutils.h
│   ├── modbus_init.h
│   ├── modbus_device.h
│   ├── uart_callbacks.h
│   └── modbus_test.h
└── Src/
    ├── main.c
    ├── modbus.c
    ├── mbutils.c
    ├── modbus_init.c
    ├── modbus_device.c
    ├── uart_callbacks.c
    ├── modbus_test.c
    ├── stm32f3xx_it.c
    └── stm32f3xx_hal_msp.c
```

## Hardware Configuration

- **Microcontroller**: STM32F303K8T6
- **UART**: USART1 (PA9 TX, PA10 RX, PA12 DE)
- **Baud Rate**: 9600
- **RS485**: Hardware flow control enabled
- **DMA**: Enabled for RX and TX

## Modbus Registers

### Holding Registers (4xxxx)

- **40001**: Sensor 1 value
- **40002**: Sensor 2 value
- **40003**: Sensor 3 value
- **40004**: Sensor 4 value
- **40005**: Temperature (×10, e.g., 205 = 20.5°C)
- **40006**: Humidity (×10, e.g., 652 = 65.2%)
- **40007**: Pressure
- **40008**: Voltage
- **40009**: Current
- **40010**: Status register
- **40011**: Error code
- **40012**: Firmware version
- **40013-40014**: Serial number
- **40015-40016**: Configuration registers
- **40017-40018**: Calibration values
- **40019**: Test register
- **40020**: Reserved

## Testing with ModbusPoll

1. **Connect Hardware**:

   - STM32 RS485 transceiver to USB-RS485 converter
   - Ensure proper termination (USB converter usually has it)

2. **Configure ModbusPoll**:

   - **Slave ID**: 1
   - **Function**: 03 (Read Holding Registers)
   - **Address**: 40001
   - **Quantity**: 10
   - **Baud Rate**: 9600
   - **Data Bits**: 8
   - **Stop Bits**: 1
   - **Parity**: None

3. **Test Operations**:
   - Read registers 40001-40010 to see test values
   - Write to register 40019 for testing
   - Monitor register 40010 for status updates

## Code Organization

### modbus_init.c

- Modbus context initialization
- UART DMA setup
- Send/receive buffer management

### modbus_device.c

- Device register read/write callbacks
- Sensor value updates
- Register mapping logic

### uart_callbacks.c

- UART receive event handling
- Error handling
- DMA restart logic

### modbus_test.c

- Test value generation
- Periodic updates
- Status simulation

## Usage

1. **Build and flash** the project
2. **Connect RS485** hardware
3. **Use ModbusPoll** to read/write registers
4. **Monitor with logic analyzer** for debugging

## Customization

- **Add Real Sensors**: Replace test values in `Modbus_Device_UpdateSensors()`
- **Modify Registers**: Update the register array in `modbus_device.c`
- **Change Baud Rate**: Update in STM32CubeMX and rebuild
- **Add Functions**: Implement additional Modbus function codes in callbacks

## Troubleshooting

- **No Response**: Check RS485 connections and termination
- **CRC Errors**: Verify baud rate and parity settings
- **Wrong Values**: Check register addresses (40001 = address 0 in array)
- **Communication Issues**: Use logic analyzer to verify signal timing
