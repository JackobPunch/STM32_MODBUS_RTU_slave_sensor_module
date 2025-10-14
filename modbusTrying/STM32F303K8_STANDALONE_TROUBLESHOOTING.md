# STM32F303K8 Standalone Mode Troubleshooting Guide

## Issue: Code Works in Debug Mode but Fails in Standalone Mode

### Symptoms

- Code runs perfectly when debugger is attached (STM32CubeIDE debug mode)
- Code hangs or fails to start when running standalone (debugger disconnected)
- LED indicators may turn on during initialization but never reach main loop
- System appears to freeze before completing startup sequence

### Root Cause Analysis

The issue stems from **system initialization state differences** between debug and standalone modes:

1. **Debug Mode**: Debugger maintains certain peripheral states and provides initialization assistance
2. **Standalone Mode**: System must initialize completely independently without debug support

## Solution 1: Syscalls Fix (Recommended for Production)

### Problem

Missing `__attribute__((weak))` on syscall functions causes conflicts with library implementations during standalone execution.

### Fix

**File**: `Core/Src/syscalls.c`

```c
// Change from:
int _read(int file, char *ptr, int len)
int _write(int file, char *ptr, int len)

// To:
__attribute__((weak)) int _read(int file, char *ptr, int len)
__attribute__((weak)) int _write(int file, char *ptr, int len)
```

### Why This Works

- Weak attributes allow linker to override these functions if needed
- Prevents conflicts with library implementations
- Ensures proper system call handling in standalone mode
- Provides long-term stability without manual intervention

## Solution 2: Power Reset Workaround (Quick Testing Solution)

### Procedure

1. Upload firmware to STM32F303K8 using STM32CubeIDE
2. **Disconnect USB cable** from the development board
3. **Reconnect USB cable** (or press reset button if available)
4. Code should now run properly in standalone mode

### Why This Works

- **Complete Power Cycle**: Clears all peripheral and power domain states
- **Clean Initialization**: Ensures fresh RAM and register initialization
- **Clock Stabilization**: Allows HSI oscillator to stabilize properly
- **Debug State Removal**: Eliminates residual debug interface states

## Comparison of Solutions

| Aspect                      | Syscalls Fix         | Power Reset                     |
| --------------------------- | -------------------- | ------------------------------- |
| **Reliability**             | High - permanent fix | Medium - requires manual action |
| **Production Suitability**  | ✅ Excellent         | ❌ Not suitable                 |
| **Development Convenience** | ✅ No manual steps   | ⚠️ Manual reset needed          |
| **Root Cause Address**      | ✅ Yes               | ⚠️ Workaround only              |
| **Implementation Time**     | 2 minutes            | 5 seconds                       |

## Recommendation

### For Production Code

**Use the syscalls fix** - it provides a permanent, robust solution that ensures reliable standalone operation regardless of power-on conditions.

### For Development/Testing

**Either solution works**, but the power reset method can be useful for:

- Quick verification that code logic is correct
- Immediate testing without code changes
- Debugging other issues while bypassing the standalone problem

### For Long-term Projects

**Implement both approaches**:

1. Apply the syscalls fix for robust operation
2. Document the power reset workaround for team members during development

## Additional Considerations

### Board-Specific Notes

- This issue appears to be specific to **NUCLEO-F303K8** boards
- Other STM32 boards may have different standalone behavior
- Custom PCBs with proper power supply design may not exhibit this issue

### Related Issues

If you encounter **Modbus communication timeouts** after fixing standalone mode:

- See `MODBUS_TIMEOUT_FIX_SUMMARY.md` for communication reliability improvements
- The timeout fixes are complementary to the standalone fixes

### Future Development

When working with STM32F303K8 projects:

1. Always test in both debug and standalone modes
2. Apply the syscalls fix early in development
3. Document power reset procedure for team members
4. Consider this behavior when selecting hardware for production

## Verification Steps

### After Applying Fixes

1. ✅ Build code successfully
2. ✅ Upload to STM32F303K8
3. ✅ Disconnect debugger
4. ✅ Verify heartbeat LED blinks (if implemented)
5. ✅ Test all functionality (Modbus, sensors, etc.)
6. ✅ Confirm sustained operation (>10 minutes)

### If Issues Persist

1. Check linker specifications (should use `--specs=nosys.specs`)
2. Verify no printf/scanf calls in code
3. Ensure proper clock configuration
4. Review interrupt priorities
5. Consider external crystal oscillator requirements

This troubleshooting guide provides both immediate and long-term solutions for STM32F303K8 standalone mode issues.
