# STM32F303K8 Modbus RTU Slave - Development Log

## Project Overview

- **MCU**: STM32F303K8T6
- **Protocol**: Modbus RTU Slave
- **Interface**: RS485 (UART1, 9600 baud, 8N1)
- **Registers**: 20 holding registers (40001-40020) with values 20 to 1
- **Status**: Works in debug mode, fails in standalone mode

## Problem Statement

The Modbus RTU slave implementation functions correctly when running in debug mode (continuous play in STM32CubeIDE), but fails to operate when the code is uploaded to the STM32 and runs standalone (without debugger attached).

## Initial Analysis (October 14, 2025)

### Codebase Review

- **main.c**: Calls `initialise_monitor_handles()` at startup (semi-hosting related)
- **All printf statements**: Commented out to prevent timing issues
- **UART Configuration**: RS485 with DMA, proper DE pin control
- **Modbus Implementation**: stModbus library, callback-based register access
- **No active printf calls**: All debug prints are disabled

### Potential Issues Identified

#### 1. Semi-hosting Related

- `initialise_monitor_handles()` is called in main.c
- Function is defined but empty in syscalls.c
- `__io_putchar` and `__io_getchar` are declared as weak externs
- In debug mode: debugger provides these functions
- In standalone mode: these functions may cause undefined behavior

#### 2. UART/DMA Configuration

- RS485 mode with DE pin (PA12)
- DMA for RX/TX operations
- UART idle line interrupt for frame detection
- Potential race conditions or initialization order issues

#### 3. Clock Configuration

- HSI oscillator, no PLL
- Standard peripheral clocks
- Possible timing differences between debug and standalone

#### 4. Memory/Stack Issues

- Stack: 1KB, Heap: 512B
- 20-element register array (40 bytes)
- Modbus buffers: 256 bytes each RX/TX

### Debug Mode vs Standalone Differences

- **Debug Mode**: Debugger provides I/O services, may mask timing issues
- **Standalone**: No debugger support, pure hardware operation

## Hypotheses

### Primary Hypothesis: Semi-hosting I/O Functions

Even though printf statements are commented out, the stdio library may still attempt to call `__io_putchar` for other operations (buffer flushing, error handling, etc.). In debug mode, the debugger provides this function. In standalone mode, it may hang or cause exceptions.

### Secondary Hypotheses

1. **UART DMA timing**: DMA operations may behave differently without debugger
2. **Clock stability**: HSI oscillator may need stabilization time
3. **RS485 DE pin timing**: GPIO toggle timing may be critical
4. **Interrupt priorities**: NVIC configuration may affect timing

## Planned Investigation Steps

### Phase 1: Semi-hosting Elimination

1. Remove `initialise_monitor_handles()` call
2. Implement stub `__io_putchar` and `__io_getchar` functions
3. Verify no stdio dependencies remain

### Phase 2: UART/DMA Verification

1. Test UART communication without Modbus
2. Verify DMA buffer handling
3. Check RS485 DE pin timing

### Phase 3: Timing Analysis

1. Add LED indicators for operation status
2. Measure execution timing differences
3. Verify clock stability

### Phase 4: Minimal Test Case

1. Create simplified version with basic UART echo
2. Gradually add Modbus components
3. Identify exact failure point

## Test Results Log

### Test 1: Code Review (October 14, 2025)

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

## Success Criteria

- **Standalone mode works**: Modbus communication functional without debugger
- **Heartbeat LED active**: PA4 toggles at 1Hz indicating main loop execution
- **No crashes/hangs**: System remains responsive to Modbus requests
- **Register values correct**: ModbusPoll reads values 20, 19, ..., 1 from registers 40001-40020

---

_Log maintained by: Development Team_
_Last updated: October 14, 2025_
