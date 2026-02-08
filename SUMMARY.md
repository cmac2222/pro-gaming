# Rust ESP - Complete Project Summary

## 📁 Project Structure

```
rust_esp/
├── Source Files (.cpp)
│   ├── main.cpp              # Application entry point & render loop
│   ├── mem.cpp               # Memory reading implementation
│   ├── overlay.cpp           # DirectX 9 overlay implementation
│   └── game_functions.cpp    # Rust-specific game logic
│
├── Header Files (.h)
│   ├── common.h              # Shared structures (vec3, Matrix4x4, bones)
│   ├── mem.h                 # Memory class definition
│   ├── overlay.h             # Overlay class definition
│   ├── game_functions.h      # Game function declarations
│   └── config.h              # Configuration & settings
│
├── Documentation
│   ├── README.md             # User guide & build instructions
│   ├── TECHNICAL.md          # Technical documentation & architecture
│   └── .gitignore           # Git ignore rules
│
└── Build System
    ├── CMakeLists.txt        # CMake build configuration
    └── build.bat             # Windows build script
```

## 🚀 Quick Start

### Prerequisites

1. **Visual Studio 2019+** (with C++ Desktop Development)
2. **CMake 3.15+**
3. **ImGui Library** - Download from: https://github.com/ocornut/imgui

### Setup Steps

```bash
# 1. Clone/download this project
cd rust_esp/

# 2. Download ImGui files and place in project root:
#    - imgui.cpp, imgui.h
#    - imgui_demo.cpp
#    - imgui_draw.cpp
#    - imgui_tables.cpp
#    - imgui_widgets.cpp
#    - imgui_impl_win32.cpp/h
#    - imgui_impl_dx9.cpp/h
#    - All other .h files from imgui/

# 3. Run build script
build.bat

# 4. Run the ESP (as Administrator)
build\Release\RustESP.exe
```

### Manual Build (Alternative)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

## 🎮 How It Works

### High-Level Overview

```
┌─────────────┐
│ Rust Game   │ ◄────┐
└─────────────┘      │
                     │ ReadProcessMemory
┌─────────────┐      │
│   ESP App   │ ─────┘
│             │
│ ┌─────────┐ │
│ │ Memory  │ │ Reads game data (entities, camera)
│ └─────────┘ │
│             │
│ ┌─────────┐ │
│ │ Overlay │ │ Transparent DirectX window
│ └─────────┘ │
│             │
│ ┌─────────┐ │
│ │  Draw   │ │ Renders skeleton ESP
│ └─────────┘ │
└─────────────┘
```

### Core Components

1. **Memory System** (`mem.h/cpp`)
   - Locates Rust process
   - Reads game memory using Windows API
   - Provides templated read functions

2. **Overlay System** (`overlay.h/cpp`)
   - Creates transparent fullscreen window
   - Initializes DirectX 9 for rendering
   - Integrates ImGui for easy drawing

3. **Game Functions** (`game_functions.h/cpp`)
   - Decrypts IL2CPP handles
   - Decrypts entity lists
   - Calculates bone positions from transforms
   - Converts world coordinates to screen space

4. **Main Application** (`main.cpp`)
   - Orchestrates all components
   - Implements caching for performance
   - Renders ESP every frame
   - Handles cleanup

## ⚙️ Configuration

Edit `config.h` to customize:

### Visual Settings
```cpp
Config::Visual::SkeletonColor = Color(255, 255, 0);  // Yellow
Config::Visual::SkeletonThickness = 2.0f;
```

### Performance
```cpp
Config::Performance::CacheUpdateInterval = 100;  // ms
Config::Performance::MaxEntitiesPerFrame = 100;
```

### Features
```cpp
Config::Features::DrawSkeleton = true;
Config::Features::DrawName = true;
```

### Game Offsets (Update after Rust patches!)
```cpp
GameOffsets::CameraClass = 0xD1D6830;
GameOffsets::BaseNetworkable = 0xD1F19B0;
GameConstants::PlayerPrefabID = 4108440852ULL;
```

## 🔧 Updating Offsets

### When Rust Updates

Offsets break with every game update. Here's how to find new ones:

#### Method 1: Using dnSpy (Easiest)
1. Download dnSpy: https://github.com/dnSpy/dnSpy
2. Open `Rust_Data/Managed/Assembly-CSharp.dll`
3. Search for class names:
   - `MainCamera` → Find camera static field offset
   - `BaseNetworkable` → Find clientEntities offset
   - `BasePlayer` → Find PrefabID constant

#### Method 2: Using IDA Pro / Ghidra
1. Open `GameAssembly.dll` in IDA/Ghidra
2. Search for string references
3. Find cross-references to static fields
4. Calculate offsets from module base

#### Method 3: Using Cheat Engine
1. Attach to RustClient.exe
2. Use "Mono" features to browse classes
3. Find static field addresses
4. Calculate offset: `address - GameAssembly.dll base`

### Update These Files

**config.h**:
```cpp
GameOffsets::CameraClass = NEW_OFFSET;
GameOffsets::BaseNetworkable = NEW_OFFSET;
```

**game_functions.cpp** (if decryption changed):
```cpp
// Update encryption constants in:
// - client_entities()
// - get_entity_list()
```

## 📊 Performance Metrics

### Expected Performance

- **CPU Usage**: 2-5%
- **Memory**: ~50MB
- **FPS Impact**: Minimal (<1 FPS)
- **Update Rate**: 10 times/second

### Optimization Tips

1. **Increase cache interval**:
   ```cpp
   Config::Performance::CacheUpdateInterval = 200; // Slower updates
   ```

2. **Limit entity processing**:
   ```cpp
   Config::Performance::MaxEntitiesPerFrame = 50;
   ```

3. **Disable unused features**:
   ```cpp
   Config::Features::DrawSkeleton = false; // If you only want names
   ```

## 🐛 Troubleshooting

### "Failed to find process: RustClient.exe"

**Solutions**:
- Ensure Rust is running
- Run ESP as Administrator
- Check exact process name in Task Manager

### "Failed to initialize overlay"

**Solutions**:
- Update graphics drivers
- Install DirectX 9 runtime
- Run as Administrator
- Disable antivirus temporarily

### ESP not visible

**Solutions**:
- Focus the game window
- Check offsets are up to date
- Verify player prefab ID
- Enable debug logging in config.h

### Skeleton looks weird/broken

**Solutions**:
- Bone offsets changed → update them
- Transform calculation changed → reverse engineer new method
- Check if bone enum matches current game version

## 📝 File Descriptions

| File | Purpose | Key Features |
|------|---------|--------------|
| `main.cpp` | Entry point | Render loop, caching, ESP drawing |
| `mem.h/cpp` | Memory reading | ReadProcessMemory wrapper, templates |
| `overlay.h/cpp` | Window overlay | DirectX 9, ImGui, transparency |
| `game_functions.h/cpp` | Game logic | Decryption, transforms, W2S |
| `common.h` | Shared types | vec3, Matrix4x4, bone enum |
| `config.h` | Settings | Colors, offsets, features |

## 🔒 Security Notes

### Detection Risk

**HIGH RISK** - This implementation uses:
- `ReadProcessMemory` (easily detected)
- Pattern-based offsets (signature scanned)
- DirectX overlay (screen capture detected)

### Recommendations

For **learning/testing only**:
✅ Use on private servers
✅ Study the code
✅ Understand anti-cheat mechanisms

**Never** use on official servers:
❌ Violates Rust Terms of Service
❌ Will result in permanent ban
❌ Ruins experience for legitimate players

### Detection Vectors

1. **Process handles** - OpenProcess calls logged
2. **Memory access patterns** - ReadProcessMemory monitored
3. **Screen capture** - Overlay detected via DirectX hooks
4. **Behavioral patterns** - Unusual player performance

## 📚 Learning Resources

### Understanding the Code

1. **IL2CPP Internals**:
   - https://github.com/djkaty/Il2CppInspector
   - Study how Unity compiles C# to native

2. **DirectX 9 Overlays**:
   - Microsoft DirectX 9 SDK documentation
   - Learn about transparent window techniques

3. **Game Hacking**:
   - Guided Hacking forums (for education)
   - GHB (Game Hacking Bible)

4. **Reverse Engineering**:
   - IDA Pro tutorials
   - Ghidra documentation
   - x86/x64 assembly basics

### Next Steps

Want to improve this project? Try:

1. **Add more features**:
   - Item ESP (boxes, dropped items)
   - Health bars
   - Distance indicators
   - FOV circle for aimbot (educational)

2. **Improve performance**:
   - Multithreading for memory reads
   - Spatial partitioning for entities
   - Custom rendering instead of ImGui

3. **Better stealth** (educational):
   - Study kernel drivers
   - Learn about DMA devices
   - Research hypervisor techniques

## 📄 License

This project is released under MIT License for **educational purposes only**.

```
MIT License

Copyright (c) 2024

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software for EDUCATIONAL PURPOSES ONLY. This software is provided
"as is" without warranty. Use at your own risk.

USING THIS SOFTWARE ON OFFICIAL GAME SERVERS IS STRICTLY PROHIBITED AND
VIOLATES THE TERMS OF SERVICE OF RUST. ANY SUCH USE IS THE SOLE
RESPONSIBILITY OF THE USER.
```

## 🤝 Contributing

Since this is for educational purposes, contributions should focus on:
- Code quality improvements
- Better documentation
- Performance optimizations
- Educational comments

**Do not contribute**:
- Features that promote cheating
- Bypass techniques for anti-cheat
- Commercial distribution methods

## ⚠️ Final Warning

**This software is for educational purposes only.**

Understanding how game hacks work is valuable for:
- Game developers (building better anti-cheat)
- Security researchers (studying vulnerabilities)
- Students (learning reverse engineering)

However, **using this on live servers is**:
- Against Rust's Terms of Service
- Disrespectful to other players
- Grounds for permanent banning
- Potentially illegal in some jurisdictions

**Code responsibly. Learn ethically.**

---

## 📞 Support

For **educational questions only**:
- Open an issue on GitHub
- Read TECHNICAL.md for deep dives
- Study the code comments

For **game bans or cheating help**:
- We don't provide support
- You accepted the risks
- Contact Facepunch at your own discretion

---

**Happy Learning! 🎓**
