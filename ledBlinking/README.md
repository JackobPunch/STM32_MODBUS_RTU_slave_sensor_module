# STM32F303K8 LED Blinking Test Project

## Overview

This project is a simple LED blinking test designed to verify that the NUCLEO-F303K8 development board works correctly in standalone mode (without debugger connection). The test addresses concerns about whether the board functions properly when not connected to a debugger, with power supplied via USB in both cases.

## Hardware

- **Board**: NUCLEO-F303K8
- **MCU**: STM32F303K8T6
- **LED Pin**: PA6 (GPIO output)
- **Power Supply**: USB (5V via ST-LINK USB connector)

## Project Configuration

- **IDE**: STM32CubeIDE / STM32CubeMX
- **HAL Library**: STM32F3xx HAL Driver
- **Clock Source**: HSI (High Speed Internal) oscillator
- **System Clock**: 8 MHz (default HSI frequency)

## GPIO Configuration

- **Pin**: PA6
- **Mode**: Output Push-Pull
- **Pull**: No pull-up/pull-down
- **Speed**: Low frequency
- **Initial State**: Reset (LOW)

## Functionality

The program implements a simple LED blinking pattern:

- **ON Duration**: 500ms
- **OFF Duration**: 500ms
- **Frequency**: 1 Hz (1 second period)
- **Pattern**: Continuous blinking in infinite loop

## Code Structure

```
Core/Src/main.c
├── System initialization (HAL_Init, SystemClock_Config)
├── GPIO initialization (MX_GPIO_Init)
└── Main loop with LED blinking logic
```

## Testing Procedure

### Debug Mode Test

1. Connect the NUCLEO-F303K8 to your computer via USB
2. Build and flash the project using your IDE
3. Run in debug mode
4. Verify LED blinking on PA6

### Standalone Mode Test

1. Keep USB connected for power supply
2. Disconnect debugger (stop debug session)
3. Press the reset button on the board
4. Observe LED blinking behavior

## Results

✅ **Test Result**: The board works correctly in both debug and standalone modes.

The LED blinks consistently at 1 Hz frequency, confirming that:

- Clock configuration is correct
- Power supply via USB is adequate
- Boot/reset sequence works properly
- No dependency on debugger connection

## Hardware Connections

- **LED Connection**: Connect LED with 330Ω current-limiting resistor between PA6 and GND
- **Power**: USB cable to ST-LINK connector (provides 5V power)
- **No external components required** for basic functionality test

## Key Features Verified

- ✅ Standalone operation capability
- ✅ USB power supply adequacy
- ✅ GPIO functionality (PA6 output)
- ✅ HAL delay functions
- ✅ System clock configuration
- ✅ Reset and boot sequence

## Code Snippets

### LED Blinking Logic

```c
while (1)
{
    // Turn LED ON
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(500);  // Wait 500ms

    // Turn LED OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_Delay(500);  // Wait 500ms
}
```

### GPIO Initialization

```c
GPIO_InitTypeDef GPIO_InitStruct = {0};
__HAL_RCC_GPIOA_CLK_ENABLE();
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

GPIO_InitStruct.Pin = GPIO_PIN_6;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
```

## Build Instructions

1. Open the project in STM32CubeIDE
2. Build the project (Ctrl+B)
3. Flash to the target board
4. Test in both debug and standalone modes

## Troubleshooting

If the board doesn't work in standalone mode:

- Check power connections
- Verify clock configuration
- Ensure proper boot pin configuration
- Check for hardware issues

## File Structure

```
ledBlinking/
├── README.md                          # This file
├── ledBlinking.ioc                   # STM32CubeMX configuration
├── STM32F303K8TX_FLASH.ld           # Linker script
├── Core/
│   ├── Inc/
│   │   ├── main.h                   # Main header file
│   │   ├── stm32f3xx_hal_conf.h     # HAL configuration
│   │   └── stm32f3xx_it.h           # Interrupt handlers header
│   ├── Src/
│   │   ├── main.c                   # Main application code
│   │   ├── stm32f3xx_hal_msp.c      # MSP initialization
│   │   ├── stm32f3xx_it.c           # Interrupt handlers
│   │   ├── syscalls.c               # System calls
│   │   ├── sysmem.c                 # System memory
│   │   └── system_stm32f3xx.c       # System initialization
│   └── Startup/
│       └── startup_stm32f303k8tx.s  # Startup assembly code
└── Drivers/                         # STM32 HAL drivers
    ├── CMSIS/                       # ARM CMSIS
    └── STM32F3xx_HAL_Driver/        # STM32F3xx HAL library
```

## Conclusion

This test successfully demonstrates that the NUCLEO-F303K8 board operates correctly in standalone mode. The simple LED blinking test provides a reliable method to verify board functionality without requiring complex peripherals or external components.

## Next Steps

With confirmed standalone operation, the board is ready for more complex projects such as:

- Sensor integration
- Modbus communication
- Real-time data acquisition
- Industrial control applications
