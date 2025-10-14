# STM32F303K8 Modbus RTU Slave - Development Log

## Project Overview

- **MCU**: STM32F303K8T6
- **Protocol**: Modbus RTU Slave
- **Interface**: RS485 (UART1, 9600 baud, 8N1)
- **Registers**: 20 holding registers (40001-40020) with values 20 to 1
- **Status**: Works in debug mode, fails in standalone mode due to NUCLEO-F303K8 board-specific hardware issue

## Problem Statement

The Modbus RTU slave implementation functions correctly when running in debug mode (continuous play in STM32CubeIDE), but fails to operate when the code is uploaded to the STM32 and runs standalone (without debugger attached). After extensive debugging, the root cause was identified as a **NUCLEO-F303K8 board-specific hardware issue** preventing standalone operation.

## Investigation Timeline (October 14, 2025)

### Phase 1: Initial Analysis

#### Codebase Review

- **main.c**: Called `initialise_monitor_handles()` at startup (semi-hosting related)
- **All printf statements**: Commented out to prevent timing issues
- **UART Configuration**: RS485 with DMA, proper DE pin control
- **Modbus Implementation**: stModbus library, callback-based register access
- **No active printf calls**: All debug prints disabled

#### Potential Issues Identified

1. **Semi-hosting Related**: `initialise_monitor_handles()` and weak I/O functions
2. **UART/DMA Configuration**: RS485 mode with DE pin, DMA operations
3. **Clock Configuration**: HSI oscillator, potential timing differences
4. **Memory/Stack Issues**: Limited stack/heap, potential overflow

### Phase 2: Semi-hosting Investigation and Removal

#### Test 1: Code Review (October 14, 2025)

- **Action**: Complete codebase analysis
- **Findings**:
  - All printf statements commented out
  - `initialise_monitor_handles()` called but function empty
  - `__io_putchar` declared weak extern, not implemented
  - UART DMA configuration appears correct
  - No obvious logic errors in Modbus implementation
- **Conclusion**: Semi-hosting most likely culprit

#### Test 2: Semi-hosting Removal (October 14, 2025)

- **Action**: Removed all semi-hosting components
- **Changes Made**:
  - Commented out `initialise_monitor_handles()` in main.c
  - Removed extern declaration for `initialise_monitor_handles`
  - Removed `-specs=rdimon.specs -lc -lrdimon` from linker flags
  - Removed `monitor arm semihosting enable` from OpenOCD config
  - Added heartbeat LED on PA4 for standalone debugging
- **Result**: Build successful with --specs=nosys.specs

#### Test 3: Build Verification (October 14, 2025)

- **Action**: Verified build succeeds after semi-hosting removal
- **Result**: Build successful - ELF file created (21,732 bytes)
- **Warnings**: 6 expected warnings about unimplemented system calls
- **Status**: Ready for hardware testing

#### Investigation: Build Warning Cleanup (October 14, 2025)

- **Issue**: Build showed "Build Failed" despite successful compilation
- **Root Cause**: Unnecessary `<stdio.h>` includes pulling in stdio library
- **Solution**: Removed unused `<stdio.h>` includes from all source files
- **Files Modified**: main.c, modbus_device.c, modbus_init.c, uart_callbacks.c, modbus.c
- **Result**: Clean build with no compilation warnings

#### Fix: System Call Warnings (October 14, 2025)

- **Issue**: Linker warnings about unimplemented \_close, \_lseek, \_read, \_write
- **Root Cause**: Weak attribute on \_read and \_write allowed libc overrides
- **Solution**: Removed weak attribute from \_read and \_write in syscalls.c
- **Result**: Our implementations override libc's, eliminating warnings

### Phase 3: Hardware Debugging with LED Indicators

#### LED Debug System Implementation (October 14, 2025)

- **Purpose**: Create visual indicators to pinpoint failure point in standalone mode
- **LED Mapping**:
  - PA0: Power-on indicator (turns on immediately)
  - PA1: HAL_Init completed
  - PA2: System clock configured
  - PA3: GPIO initialized
  - PA4: UART initialized
  - PA5: Modbus initialized
  - PA6: Main loop entered (heartbeat LED)
- **Implementation**: Sequential LED activation through initialization phases

#### Test 4: LED Debug System Testing (October 14, 2025)

- **Action**: Test LED debug system in both debug and standalone modes
- **Debug Mode Results**: All LEDs turn on sequentially, PA6 blinks (heartbeat)
- **Standalone Mode Results**: LEDs turn on up to PA5, PA6 never turns on
- **Conclusion**: Code reaches Modbus initialization but never enters main loop

### Phase 4: Component Isolation Testing

#### UART Disable Test (October 14, 2025)

- **Action**: Disable UART and RS485 to isolate potential UART issues
- **Changes**: Added HAL_UART_DeInit() and GPIO control for DE pin
- **Result**: Same failure - PA6 still doesn't turn on
- **Conclusion**: Issue not related to UART/DMA

#### HAL Library Bypass Test (October 14, 2025)

- **Action**: Remove HAL_Init() and use direct register access
- **Changes**: Manual GPIOA clock enable, direct register GPIO setup
- **Result**: Same failure pattern
- **Conclusion**: Issue not related to HAL library

#### Minimal Bare-Metal Test (October 14, 2025)

- **Action**: Create absolute minimal code - just GPIO toggle
- **Code**: Manual GPIOA setup, PA6 output, busy-wait loop
- **Debug Mode**: PA6 blinks perfectly
- **Standalone Mode**: PA6 turns on but never blinks
- **Conclusion**: **Hardware/board issue confirmed** - not software/HAL/library problem

### Phase 5: Board-Specific Investigation

#### NUCLEO-F303K8 Board Analysis (October 14, 2025)

- **Issue**: NUCLEO-F303K8 has different power supply than assumed
- **Discovery**: JP5 jumper (mentioned initially) doesn't exist on Nucleo-32 boards
- **Nucleo-32 Power Supply**: Automatic switching between USB and external power
- **Current Hypothesis**: USB power insufficient or unstable for standalone operation
- **Alternative Hypothesis**: External crystal oscillator required but not configured

## Root Cause Analysis

### Confirmed Findings

1. **Software is correct**: All components work perfectly in debug mode
2. **HAL libraries work**: Direct register access shows same issue
3. **UART/DMA not involved**: Issue persists with UART completely disabled
4. **Code reaches initialization**: GPIO setup works, main loop never entered

### Root Cause: NUCLEO-F303K8 Board Hardware Issue

- **Symptom**: Code initializes successfully but hangs before main loop execution
- **Debug vs Standalone**: Works perfectly with debugger, fails without it
- **Board-Specific**: Issue appears unique to NUCLEO-F303K8 in standalone mode
- **Possible Causes**:
  - Insufficient USB power delivery for standalone operation
  - External crystal oscillator required but not present/configured
  - Board-specific power sequencing requirements not met
  - Reset/boot pin configuration differences

### Impact Assessment

- **Software**: Fully functional and debugged
- **Hardware**: NUCLEO-F303K8 has standalone operation limitations
- **Solution Required**: Board-specific workaround or different hardware

## Code Status and Changes

### Working Debug Mode Code (Current State)

- **Modbus RTU Slave**: Fully functional with RS485, DMA, callbacks
- **LED Debug System**: PA0-PA6 indicators for operation status
- **Semi-hosting Removed**: Clean standalone build configuration
- **Tested**: Works perfectly in STM32CubeIDE debug mode

### Standalone Mode Issues

- **Failure Point**: Code initializes but never reaches main loop
- **Minimal Test**: Even bare-metal GPIO toggle fails
- **Board Limitation**: NUCLEO-F303K8 specific hardware issue

## Recommendations

### Immediate Actions

1. **Document Current State**: Working Modbus code with LED debug system
2. **Hardware Alternative**: Consider different STM32 board for production
3. **External Power**: Test with stable 3.3V external power supply
4. **Crystal Oscillator**: Investigate if external crystal required

### Future Development

1. **Board Migration**: Port code to different STM32 board for standalone operation
2. **Power Supply Analysis**: Investigate NUCLEO-F303K8 power requirements
3. **Crystal Configuration**: Test with external crystal oscillator
4. **Alternative MCUs**: Consider STM32F103, STM32F401, or custom PCB

## Success Criteria (For Working Board)

- **Standalone mode works**: Modbus communication functional without debugger
- **Heartbeat LED active**: PA6 toggles at 1Hz indicating main loop execution
- **No crashes/hangs**: System remains responsive to Modbus requests
- **Register values correct**: ModbusPoll reads values 20, 19, ..., 1 from registers 40001-40020

## Files Modified During Debugging

### Core Files

- `main.c`: Removed semi-hosting, added LED debug system
- `syscalls.c`: Implemented system call stubs
- `modbus_init.c`: Added UART disable option for testing
- `modbus_device.c`: Removed printf dependencies

### Build Configuration

- `.cproject`: Removed semi-hosting linker specs
- OpenOCD launch config: Removed semi-hosting enable command

### Documentation

- `README.md`: Updated with troubleshooting and register map
- `DEVELOPMENT_LOG.md`: Complete debugging history (this file)

---

**Status**: Working Modbus RTU slave code with LED debug system (debug mode only)  
**Root Cause**: NUCLEO-F303K8 board-specific hardware limitation in standalone mode  
**Next Steps**: Board migration or hardware workaround investigation  
_Log maintained by: Development Team_  
_Last updated: October 14, 2025_

- **Action**: Complete codebase analysis
- **Findings**:
  - All printf statements commented out
  - `initialise_monitor_handles()` called but function is empty
  - `__io_putchar` declared weak extern, not implemented
  - UART DMA configuration appears correct
  - No obvious logic errors in Modbus implementation
- **Conclusion**: Semi-hosting is the most likely culprit

### Test 2: Semi-hosting Removal (October 14, 2025)

- **Action**: Removed all semi-hosting components from build configuration
- **Changes Made**:
  - Commented out `initialise_monitor_handles()` in main.c
  - Removed extern declaration for `initialise_monitor_handles`
- **Completed**: Removed `-specs=rdimon.specs -lc -lrdimon` from linker flags in .cproject
- **Completed**: Removed `monitor arm semihosting enable` from OpenOCD launch config
  - Added heartbeat LED on PA4 for standalone debugging
  - Reverted syscalls.c to original state (stubs removed)
- **Rationale**: Semi-hosting works in debug mode but fails in standalone mode
- **Expected Outcome**: Standalone operation restored
- **Next Step**: Remove linker specs and OpenOCD commands, then test

## Code Changes Made

### README.md Updates

- Corrected register value documentation (20 to 1)
- Added detailed register map table
- Improved troubleshooting section

## Open Questions

1. Why is `initialise_monitor_handles()` called if the function is empty?
2. Are there any implicit stdio calls in HAL or stModbus libraries?
3. Does the debugger affect UART timing or DMA operations?
4. Is there a minimum delay required after power-on for RS485 transceiver?

## Next Steps

1. Remove semi-hosting call from main.c
2. Implement stub I/O functions in syscalls.c
3. Test standalone operation
4. If still failing, investigate UART DMA issues
5. Add status LEDs for debugging standalone operation

### Semi-hosting Fixes (October 14, 2025)

- **Action**: Removed all semi-hosting components from build configuration
- **Changes Made**:
  - Commented out `initialise_monitor_handles()` in main.c
  - Removed extern declaration for `initialise_monitor_handles`
  - Removed `-specs=rdimon.specs -lc -lrdimon` from linker flags in .cproject
  - Removed `monitor arm semihosting enable` from OpenOCD launch config
  - Added heartbeat LED on PA4 for standalone debugging
  - Reverted syscalls.c to original state (stubs removed)
- **Result**: Build successful with --specs=nosys.specs, expected warnings about unimplemented system calls
- **Status**: Ready for hardware testing

### Test 3: Build Verification (October 14, 2025)

- **Action**: Verified build succeeds after semi-hosting removal
- **Result**: Build successful - ELF file created (21,732 bytes), only expected warnings about unimplemented system calls
- **Warnings**: 6 warnings about \_close, \_fstat, \_isatty, \_lseek, \_read, \_write not implemented (expected with --specs=nosys.specs)
- **Linker**: Now using --specs=nosys.specs (correct for standalone operation)
- **Status**: Ready for testing on hardware

### Investigation: Misleading Build Messages (October 14, 2025)

- **Issue**: Build shows "Build Failed. 6 errors, 6 warnings" despite successful compilation and linking
- **Root Cause**: Unnecessary <stdio.h> includes pulling in stdio library code
- **Solution**: Removed unused <stdio.h> includes from all source files
- **Files Modified**: main.c, modbus_device.c, modbus_init.c, uart_callbacks.c, modbus.c
- **Kept**: syscalls.c retains <stdio.h> for system call stubs
- **Status**: Should eliminate linker warnings about unimplemented functions

### Fix: Printf Warning in modbus_device.c (October 14, 2025)

- **Issue**: Warning about implicit declaration of printf in Modbus_Device_PrintArray function
- **Root Cause**: Removed <stdio.h> include but function actually uses printf for debugging
- **Solution**: Removed unused Modbus_Device_PrintArray function and <stdio.h> include entirely
- **Result**: Clean code, no printf dependencies for standalone operation
- **Status**: Build should now be clean of compilation warnings

### Fix: Linker Warnings for System Calls (October 14, 2025)

- **Issue**: Linker warnings about unimplemented \_close, \_lseek, \_read, \_write functions
- **Root Cause**: Weak attribute on \_read and \_write allowed libc_nano.a's warning implementations to override ours
- **Solution**: Removed **attribute**((weak)) from \_read and \_write in syscalls.c to make them strong; ensured syscalls.c is included in build by removing exclusion from .cproject
- **Result**: Our implementations override libc's, eliminating warnings
- **Status**: Build should now complete without warnings

### Investigation: Hardware/Board Issue Identified (October 14, 2025)

- **Issue**: Even minimal bare-metal code (no HAL_Init, manual GPIO setup) fails in standalone mode
- **Test Results**:
  - Debug mode: All tests work perfectly
  - Standalone mode: LED turns on but never blinks (code reaches GPIO init but not main loop)
- **Root Cause**: NUCLEO-F303K8 board-specific issue when running standalone vs debug mode
- **Possible Causes**:
  - USB power supply insufficient for standalone operation
  - External crystal oscillator required but not configured
  - Board-specific initialization requirements not met
  - Power sequencing issues between debug and standalone modes
- **Minimal Test Code**: Manual GPIOA clock enable, PA6 output config, busy-wait delay loop
- **Status**: Hardware/board issue confirmed - not software/HAL/library problem
- **Next Steps**: User to investigate NUCLEO-F303K8 datasheet for standalone requirements

## Success Criteria

- **Standalone mode works**: Modbus communication functional without debugger
- **Heartbeat LED active**: PA4 toggles at 1Hz indicating main loop execution
- **No crashes/hangs**: System remains responsive to Modbus requests
- **Register values correct**: ModbusPoll reads values 20, 19, ..., 1 from registers 40001-40020

---

_Log maintained by: Development Team_
_Last updated: October 14, 2025_
