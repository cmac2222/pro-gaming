# Rust ESP - External Overlay

A clean, performant external ESP overlay for Rust using DirectX 9 and ImGui.

## Features

- **Skeleton ESP**: Draws player bone structures
- **Player Labels**: Shows "Player" text above heads
- **Optimized Rendering**: Cached game data to minimize memory reads
- **Clean Architecture**: Separated concerns (memory, overlay, game logic)
- **World-to-Screen**: Accurate projection using view matrix

## Project Structure

```
rust_esp/
├── main.cpp              # Main application loop
├── mem.h/cpp             # Memory reading (ReadProcessMemory wrapper)
├── overlay.h/cpp         # DirectX 9 overlay window
├── game_functions.h/cpp  # Rust-specific decryption & bone logic
├── common.h              # Shared structures (vec3, Matrix4x4, bones)
└── CMakeLists.txt        # Build configuration
```

## Requirements

### Software
- Windows 10/11
- Visual Studio 2019+ (or any C++17 compiler)
- CMake 3.15+
- DirectX 9 SDK (included in Windows SDK)

### Libraries
- **ImGui** - Download from https://github.com/ocornut/imgui
  - Required files:
    - imgui.cpp
    - imgui_demo.cpp
    - imgui_draw.cpp
    - imgui_tables.cpp
    - imgui_widgets.cpp
    - imgui_impl_win32.cpp
    - imgui_impl_dx9.cpp
    - All .h header files

## Setup Instructions

### 1. Download ImGui

```bash
git clone https://github.com/ocornut/imgui.git
```

Copy these files from `imgui/` and `imgui/backends/` into your project directory:
- `imgui/*.cpp` and `imgui/*.h`
- `imgui/backends/imgui_impl_win32.cpp` and `.h`
- `imgui/backends/imgui_impl_dx9.cpp` and `.h`

### 2. Build with Visual Studio

#### Option A: Using CMake
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

#### Option B: Manual Visual Studio Project
1. Create new C++ project
2. Add all `.cpp` and `.h` files
3. Project Properties → Linker → Input → Additional Dependencies:
   - `d3d9.lib`
   - `dwmapi.lib`
4. Set C++ Standard to C++17 or later
5. Build in Release mode

### 3. Run

1. Launch Rust
2. Run `RustESP.exe` as Administrator
3. The overlay will appear over Rust once the game window is focused

## Configuration

### Update Offsets

Offsets are located in `main.cpp`:

```cpp
namespace Offsets {
    constexpr uintptr_t CameraClass = 0xD1D6830;      // Update this
    constexpr uintptr_t BaseNetworkable = 0xD1F19B0;  // Update this
    constexpr uintptr_t IL2CPPBase = 0xD4C3CF0;       // Update this
}
```

These offsets change with each Rust update. Use a tool like Cheat Engine or IDA Pro to find updated offsets.

### Adjust Player Prefab ID

If players aren't being detected, update the prefab ID in `main.cpp`:

```cpp
constexpr uint32_t PLAYER_PREFAB_ID = 4108440852ULL;
```

## Code Architecture

### Memory System (`mem.h/cpp`)
- Uses `ReadProcessMemory` for external reading
- Template-based reading for type safety
- Chain reading for pointer paths

### Overlay System (`overlay.h/cpp`)
- DirectX 9 transparent overlay
- ImGui integration
- Layered window for click-through

### Game Functions (`game_functions.h/cpp`)
- IL2CPP handle decryption
- Entity list decryption
- Transform matrix calculations
- Bone position extraction
- World-to-screen projection

### Main Loop (`main.cpp`)
- Cached game data (updates every 100ms)
- Efficient ESP rendering
- Device loss recovery
- Clean shutdown

## Performance Notes

1. **Caching**: Game data is cached and updated every 100ms to reduce memory reads
2. **Culling**: Players off-screen are skipped
3. **Batch Operations**: All drawing done in single ImGui frame
4. **Sleep**: 1ms sleep prevents 100% CPU usage

## Troubleshooting

### "Failed to find process: RustClient.exe"
- Ensure Rust is running
- Run as Administrator
- Check process name matches exactly

### "Failed to initialize overlay"
- Update graphics drivers
- Ensure DirectX 9 is installed
- Try running as Administrator

### No ESP visible
- Check if game window is focused
- Verify offsets are correct for your Rust version
- Ensure player prefab ID is correct

### ESP flickering
- Increase cache update interval
- Check for device loss recovery issues

## Legal Disclaimer

**Educational purposes only.** This code is provided for learning about:
- External process memory reading
- DirectX overlay techniques
- Game reverse engineering concepts

Using this on official Rust servers will result in a ban. Use at your own risk.

## Credits

- ImGui: https://github.com/ocornut/imgui
- Reverse engineering techniques from public research

## License

MIT License - See LICENSE file for details
