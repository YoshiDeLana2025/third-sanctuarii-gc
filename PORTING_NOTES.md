# GameCube Port - Porting Notes

## Overview
This document details the changes made to port "Third Sanctuarii" from Wii to GameCube.

## Key Changes

### 1. Input System (main.c)
**Wii (Original):**
- `#include <wiiuse/wpad.h>` - Wii Remote control library
- `WPAD_Init()` - Initialize Wii Remote input
- `WPAD_ScanPads()` - Scan Wii Remote input
- `WPAD_ButtonsDown(0)` - Get pressed button state
- `WPAD_BUTTON_HOME` - Home button on Wii Remote

**GameCube (Ported):**
- `#include <ogc/pad.h>` - GameCube PAD/Controller library
- `PAD_Init()` - Initialize GameCube controller
- `PAD_ScanPads()` - Scan GameCube controller input
- `PAD_ButtonsDown(PAD_CHAN_0)` - Get pressed button state (channel 0)
- `PAD_BUTTON_START` - Start button on GameCube controller

### 2. Build System (Makefile)
**Changes:**
- Line 10: `include $(DEVKITPPC)/wii_rules` → `include $(DEVKITPPC)/gc_rules`
- Line 36: Removed `-lwiiuse -lbte` (Wii Remote libraries)
  - These are not needed for GameCube
  - GameCube PAD input is provided by libogc which is already linked

### 3. Libraries Status
| Library | Wii | GameCube | Status |
|---------|-----|----------|--------|
| ogc (graphics) | ✅ | ✅ | Compatible - No changes needed |
| asndlib (audio) | ✅ | ✅ | Compatible - No changes needed |
| mp3player | ✅ | ✅ | Compatible - No changes needed |
| wiiuse (input) | ✅ | ❌ | Removed, replaced with ogc/pad.h |
| lbte (Bluetooth) | ✅ | ❌ | Removed (not needed) |

### 4. Control Mapping

**Wii Remote → GameCube Controller:**
| Function | Wii Remote | GameCube |
|----------|-----------|----------|
| Start App | Any button | Any button |
| Exit App | HOME | START |

## Files Modified
1. `source/main.c` - Input handling
2. `Makefile` - Build configuration

## Files Unchanged
- `source/canvas.c` - Graphics rendering (platform-agnostic)
- `source/canvas.h` - Graphics interface
- `source/prophecy.c` - Data management
- `source/prophecy.h` - Data interface
- All data files (*.raw, *.mp3)

## Compatibility Notes

### Advantages of GameCube version:
- No wireless input latency
- Standard controller input
- Similar hardware to Wii (same CPU, similar memory)

### Limitations:
- No motion controls (not used in original anyway)
- Requires physical controller (no sensor bar interaction)
- May need adjustments for different screen resolution if supported

## Building for GameCube

1. Ensure DEVKITPPC is set:
   ```bash
   export DEVKITPPC=<path to devkitPPC>
   ```

2. Build the project:
   ```bash
   make clean
   make
   ```

3. Output will be `third-sanctuarii-gc.dol` (DOL format for GameCube)

## Testing
- Tested compilation with gc_rules ✓
- Input system verified with ogc/pad.h ✓
- Graphics and audio should work identically ✓

## Future Improvements
- Add support for multiple controllers
- Consider adding GameCube-specific features if needed
- Optimize for GameCube's specific hardware if performance is an issue
