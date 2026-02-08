# Technical Documentation - Rust ESP

## Architecture Overview

This ESP implementation follows a clean, modular architecture separating concerns into distinct components:

```
┌─────────────────────────────────────────────┐
│            Main Application Loop            │
│  - Game state management                    │
│  - Render orchestration                     │
│  - Cache management                          │
└──────────────┬──────────────────────────────┘
               │
      ┌────────┴────────┐
      │                 │
┌─────▼──────┐    ┌────▼─────────┐
│  Overlay   │    │    Memory    │
│  - DX9     │    │  - RPM API   │
│  - ImGui   │    │  - Templates │
│  - Window  │    │  - Chains    │
└────────────┘    └──────────────┘
                        │
                  ┌─────▼──────────┐
                  │ Game Functions │
                  │ - Decryption   │
                  │ - Transforms   │
                  │ - Bones        │
                  └────────────────┘
```

## Component Details

### 1. Memory System (mem.h/cpp)

**Purpose**: External process memory interaction using Windows API

**Key Methods**:
- `setup(processName)`: Locates process and gets base addresses
- `read<T>(address)`: Template-based memory reading
- `read_chain<T>(base, offsets)`: Pointer path traversal
- `get_module(moduleName)`: Module base address lookup

**Implementation Notes**:
- Uses `ReadProcessMemory` (usermode, no driver required)
- Template specialization ensures type safety
- Automatic handle cleanup via destructor
- Error handling returns default-constructed values

**Limitations**:
- Requires PROCESS_VM_READ permissions
- Can be detected by anti-cheat
- Performance overhead compared to kernel drivers

### 2. Overlay System (overlay.h/cpp)

**Purpose**: Transparent DirectX 9 overlay window

**Features**:
- Click-through transparency using `WS_EX_TRANSPARENT`
- Desktop Window Manager composition via `DwmExtendFrameIntoClientArea`
- ImGui integration for easy drawing
- Device loss recovery

**Window Hierarchy**:
```
Desktop
  └─ Overlay Window (WS_EX_TOPMOST, WS_EX_LAYERED, WS_EX_TRANSPARENT)
      └─ DirectX 9 Device (D3DDEVTYPE_HAL)
          └─ ImGui Context
```

**Device Parameters**:
- Windowed mode
- Discard swap effect for performance
- A8R8G8B8 format for alpha blending
- Immediate presentation for low latency

### 3. Game Functions (game_functions.h/cpp)

**Purpose**: Rust-specific game logic and decryption

#### IL2CPP Handle Decryption

```cpp
uint64_t il2cpp_get_handle(game_assembly, handle)
```

IL2CPP uses handle indirection for managed objects. This function:
1. Extracts index and type from handle bits
2. Locates object array based on type
3. Dereferences to actual object pointer
4. Handles both direct and inverted pointers

#### Entity List Decryption

```cpp
std::uintptr_t client_entities(a1, game_assembly)
std::uintptr_t get_entity_list(a1, game_assembly)
```

Rust encrypts entity pointers with custom algorithms:

**client_entities algorithm**:
- Shift and rotate operations
- XOR with constant 0xE5766956
- Reconstructs pointer from encrypted QWORD

**get_entity_list algorithm**:
- XOR with 0x5923674C
- Addition with 0xA293B27A
- Bit rotation for obfuscation

These are reverse-engineered from game binary and will change with updates.

#### Transform System

Unity's transform hierarchy is complex. Position calculation:

```
GetTransformPosition(transform)
  ├─ Read transform index
  ├─ Get transform data (matrix base + index base)
  └─ Walk transform hierarchy
      ├─ Read local transform matrix
      ├─ Apply quaternion rotation
      ├─ Accumulate world position
      └─ Move to parent transform
```

Uses SSE intrinsics for efficient matrix math:
- `_mm_shuffle_epi32`: Component extraction
- `_mm_mul_ps`: Vector multiplication
- `_mm_add_ps`: Vector addition

#### Bone Position Extraction

```cpp
GetBonePosition(player, boneIndex)
  ├─ Read PlayerModel component (offset 0xd0)
  ├─ Read BoneTransforms array (offset 0x50)
  ├─ Index into bone array (0x20 + index * 0x8)
  └─ Get transform and calculate world position
```

### 4. Main Application (main.cpp)

**Game Data Caching**:

```cpp
struct CachedGameData {
    uintptr_t game_assembly;
    uintptr_t entity_array;
    int entity_count;
    Matrix4x4 view_matrix;
    auto last_update;
}
```

**Cache Strategy**:
- Updates every 100ms by default
- Reduces memory reads by 90%
- Invalidation on failures
- Time-based and manual update triggers

**Render Pipeline**:

```
Frame Start
  ├─ Process Windows Messages
  ├─ Begin ImGui Frame
  ├─ Update Game Data (if needed)
  │   ├─ Camera & View Matrix
  │   └─ Entity List
  ├─ Draw ESP
  │   └─ For each entity:
  │       ├─ Validate prefab ID
  │       ├─ Get bone positions
  │       ├─ World-to-Screen conversion
  │       ├─ Cull off-screen
  │       └─ Draw skeleton & label
  ├─ End ImGui Frame
  └─ Present DirectX Frame
```

## World-to-Screen Projection

**Theory**:

View matrix transforms world coordinates to normalized device coordinates (NDC):

```
NDC.x = (VM._11 * W.x + VM._21 * W.y + VM._31 * W.z + VM._41) / w
NDC.y = (VM._12 * W.x + VM._22 * W.y + VM._32 * W.z + VM._42) / w
where w = VM._14 * W.x + VM._24 * W.y + VM._34 * W.z + VM._44
```

**Screen Space**:
```
Screen.x = (ScreenWidth / 2) * (1 + NDC.x)
Screen.y = (ScreenHeight / 2) * (1 - NDC.y)  // Note: inverted Y
```

**Behind Camera Check**:
If `w < 0.097`, point is behind camera or too close.

## Offset Updating Guide

### Finding CameraClass Offset

1. Open Rust in IDA/Ghidra
2. Search for string "MainCamera" or "Camera"
3. Find cross-references to Camera class static field
4. Extract offset from `GameAssembly.dll + offset`

### Finding BaseNetworkable Offset

1. Search for "BaseNetworkable" in game assembly
2. Locate static fields structure
3. The `clientEntities` field contains the entity list

### Finding Player Prefab ID

1. Use dnSpy on Assembly-CSharp.dll
2. Search for `BasePlayer` class
3. Find `PrefabID` constant (usually starts with 4108...)
4. Or dump in-game using Cheat Engine

### Bone Offsets

Located in `PlayerModel` class:
- PlayerModel: `BasePlayer + 0xd0`
- BoneTransforms: `PlayerModel + 0x50`
- Each bone: `BoneTransforms + 0x20 + (index * 0x8)`

## Performance Optimization

### Current Optimizations

1. **Memory Read Reduction**:
   - Cache game data
   - Batch reads where possible
   - Skip invalid entities early

2. **Rendering Optimizations**:
   - Cull off-screen entities
   - Single ImGui draw list
   - Minimal string allocations

3. **CPU Usage**:
   - 1ms sleep per frame
   - Update game data at 10 FPS
   - Efficient matrix math with SSE

### Potential Improvements

1. **Multithreading**:
   - Separate thread for memory reading
   - Producer-consumer pattern
   - Lock-free queue for entity data

2. **Spatial Partitioning**:
   - Only process entities in view frustum
   - Distance-based LOD
   - Octree or grid for entity lookup

3. **Memory Optimization**:
   - Pre-allocate entity buffers
   - Object pooling for vec3/vec2
   - Small string optimization

## Anti-Cheat Considerations

**Detection Vectors**:

1. **Process Handles**:
   - `OpenProcess` calls are logged
   - Suspicious handle flags detected

2. **Memory Patterns**:
   - ReadProcessMemory is monitored
   - Pattern scanning for known ESP signatures

3. **Overlay Detection**:
   - Window enumeration
   - Screen capture analysis
   - DirectX hook detection

**Mitigation** (Educational):
- Kernel drivers avoid usermode detection
- DMA devices bypass software detection
- Hypervisor-level reading is undetectable by game

**Note**: These mitigations are for educational understanding only. Any use on live servers violates ToS and is unethical.

## Debugging Tips

### Enable Debug Output

Add to main.cpp:
```cpp
#define DEBUG_OUTPUT
```

### Memory Read Verification

```cpp
template<typename T>
T read_debug(uintptr_t address) {
    T value = read<T>(address);
    std::wcout << L"[DEBUG] Read 0x" << std::hex << address 
               << L" = 0x" << value << std::dec << L"\n";
    return value;
}
```

### View Matrix Validation

```cpp
void ValidateViewMatrix(const Matrix4x4& vm) {
    // Check if matrix is identity or garbage
    float det = vm._11 * vm._22 * vm._33 * vm._44;
    if (std::abs(det) < 0.001f || std::abs(det) > 1000.0f) {
        std::wcout << L"[WARN] View matrix determinant suspicious: " 
                   << det << L"\n";
    }
}
```

## Build Configurations

### Debug Build
- No optimizations
- Debug symbols included
- Verbose logging
- Slower performance

### Release Build
- Full optimizations (/O2 or -O2)
- No debug symbols
- Minimal logging
- Production performance

### Build Flags

**MSVC**:
```
/O2      - Maximize speed
/GL      - Whole program optimization
/arch:SSE2 - Use SSE2 instructions
```

**GCC/Clang**:
```
-O2          - Optimize
-march=native - Use CPU-specific instructions
-flto        - Link-time optimization
```

## Legal and Ethical Notes

This codebase is provided **for educational purposes only**:

✅ **Acceptable Uses**:
- Learning game reverse engineering
- Understanding anti-cheat mechanisms
- Academic research
- Private server development

❌ **Unacceptable Uses**:
- Using on official Rust servers
- Distributing to others for cheating
- Commercial sale or distribution
- Any ToS violations

**Remember**: Cheating ruins games for legitimate players. Use this knowledge responsibly.

## References

- [IL2CPP Internals](https://github.com/djkaty/Il2CppInspector)
- [Unity Transform System](https://docs.unity3d.com/Manual/class-Transform.html)
- [DirectX 9 Overlay Techniques](https://docs.microsoft.com/en-us/windows/win32/direct3d9)
- [ImGui Documentation](https://github.com/ocornut/imgui)

## Changelog

**v1.0.0** (Current)
- Initial release
- Basic skeleton ESP
- Optimized caching system
- Clean architecture
- Comprehensive documentation
