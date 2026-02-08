#pragma once

// ===========================================
// ESP Configuration
// ===========================================

namespace Config {
    // Visual Settings
    namespace Visual {
        // ESP Colors (RGBA)
        struct Color {
            int r, g, b, a;
            Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
        };

        inline Color SkeletonColor = Color(255, 255, 0, 255);  // Yellow
        inline Color NameColor = Color(255, 255, 0, 255);       // Yellow
        inline Color BoxColor = Color(255, 0, 0, 255);          // Red

        // Line thickness
        inline float SkeletonThickness = 2.0f;
        inline float BoxThickness = 2.0f;

        // Font size
        inline float FontSize = 14.0f;
    }

    // Performance Settings
    namespace Performance {
        // Cache update interval in milliseconds
        inline int CacheUpdateInterval = 100;  // 10 FPS update rate

        // Frame limiter (0 = unlimited)
        inline int MaxFPS = 0;

        // Max entities to process per frame
        inline int MaxEntitiesPerFrame = 100;
    }

    // Feature Toggles
    namespace Features {
        inline bool DrawSkeleton = true;
        inline bool DrawName = true;
        inline bool DrawBox = false;
        inline bool DrawDistance = false;
        inline bool DrawHealth = false;
    }

    // Distance Limits
    namespace Distance {
        // Max render distance (in meters)
        inline float MaxRenderDistance = 500.0f;

        // Show distance text
        inline bool ShowDistance = false;
    }

    // Debug Settings
    namespace Debug {
        // Enable console output
        inline bool EnableLogging = true;

        // Verbose logging
        inline bool VerboseLogging = false;

        // Show FPS counter
        inline bool ShowFPS = false;

        // Show entity count
        inline bool ShowEntityCount = false;
    }

    // Hotkeys (Virtual Key Codes)
    namespace Hotkeys {
        inline int ToggleESP = VK_INSERT;       // INSERT key
        inline int ToggleSkeleton = VK_F1;      // F1 key
        inline int ToggleBox = VK_F2;           // F2 key
        inline int Exit = VK_END;               // END key
    }
}

// ===========================================
// Game Offsets (Update these regularly!)
// ===========================================

namespace GameOffsets {
    // Base offsets (from GameAssembly.dll)
    inline uintptr_t CameraClass = 0xD1D6830;
    inline uintptr_t BaseNetworkable = 0xD1F19B0;
    inline uintptr_t IL2CPPBase = 0xD4C3CF0;

    // Component offsets
    namespace Component {
        inline uintptr_t PlayerModel = 0xd0;
        inline uintptr_t BoneTransforms = 0x50;
        inline uintptr_t BoneArray = 0x20;
    }

    // Entity offsets
    namespace Entity {
        inline uintptr_t PrefabID = 0x30;
        inline uintptr_t Transform = 0x10;
    }

    // Camera offsets
    namespace Camera {
        inline uintptr_t StaticFields = 0xb8;
        inline uintptr_t MainCamera = 0x50;
        inline uintptr_t Parent = 0x10;
        inline uintptr_t ViewMatrix = 0x30C;
    }

    // Transform offsets
    namespace Transform {
        inline uintptr_t TransformData = 0x38;
        inline uintptr_t Index = 0x40;
        inline uintptr_t MatrixBase = 0x18;
        inline uintptr_t IndexBase = 0x20;
    }
}

// ===========================================
// Game Constants
// ===========================================

namespace GameConstants {
    // Prefab IDs (these change less often)
    inline uint32_t PlayerPrefabID = 4108440852ULL;
    
    // You can add more prefab IDs here:
    // inline uint32_t SupplyDropPrefabID = 1234567890ULL;
    // inline uint32_t HelicopterPrefabID = 9876543210ULL;
}

// ===========================================
// Screen Settings
// ===========================================

namespace Screen {
    // Auto-detect or manual override
    inline bool AutoDetectResolution = true;
    inline int Width = 1920;
    inline int Height = 1080;
}
