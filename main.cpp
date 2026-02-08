#include <iostream>
#include <chrono>
#include "mem.h"
#include "overlay.h"
#include "game_functions.h"
#include "config.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

// Global memory instance
Kmem mem{};

struct CachedGameData {
    uintptr_t game_assembly = 0;
    uintptr_t entity_array = 0;
    int entity_count = 0;
    Matrix4x4 view_matrix{};
    
    std::chrono::steady_clock::time_point last_update;
    bool needs_update = true;
    
    void MarkNeedsUpdate() {
        needs_update = true;
    }
    
    bool ShouldUpdate(int ms_interval = 100) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
        return needs_update || elapsed >= ms_interval;
    }
    
    void MarkUpdated() {
        last_update = std::chrono::steady_clock::now();
        needs_update = false;
    }
};

bool UpdateGameData(CachedGameData& cache) {
    if (!cache.ShouldUpdate(Config::Performance::CacheUpdateInterval)) {
        return true;  // Use cached data
    }
    
    try {
        // Get camera and view matrix
        uintptr_t camera_class = mem.read<uintptr_t>(cache.game_assembly + GameOffsets::CameraClass);
        if (!camera_class) return false;
        
        uintptr_t cam_static_field = mem.read<uintptr_t>(camera_class + GameOffsets::Camera::StaticFields);
        if (!cam_static_field) return false;
        
        uintptr_t cam_obj = mem.read<uintptr_t>(cam_static_field + GameOffsets::Camera::MainCamera);
        if (!cam_obj) return false;
        
        uintptr_t cam_parent = mem.read<uintptr_t>(cam_obj + GameOffsets::Camera::Parent);
        if (!cam_parent) return false;
        
        cache.view_matrix = mem.read<Matrix4x4>(cam_parent + GameOffsets::Camera::ViewMatrix);
        
        // Get entity list
        uintptr_t base_networkable = mem.read<uintptr_t>(cache.game_assembly + GameOffsets::BaseNetworkable);
        if (!base_networkable) return false;
        
        uintptr_t static_fields = mem.read<uintptr_t>(base_networkable + 0xb8);
        if (!static_fields) return false;
        
        uintptr_t entity_ptr = mem.read<uintptr_t>(static_fields + 0x30);
        if (!entity_ptr) return false;
        
        uintptr_t client_entity = client_entities(mem, entity_ptr, cache.game_assembly);
        if (!client_entity) return false;
        
        uintptr_t entity_realm = mem.read<uintptr_t>(client_entity + 0x10);
        if (!entity_realm) return false;
        
        uintptr_t entity_list = get_entity_list(mem, entity_realm, cache.game_assembly);
        if (!entity_list) return false;
        
        uintptr_t buffer_list = mem.read<uintptr_t>(entity_list + 0x18);
        if (!buffer_list) return false;
        
        cache.entity_count = mem.read<int>(buffer_list + 0x18);
        cache.entity_array = mem.read<uintptr_t>(buffer_list + 0x10);
        
        cache.MarkUpdated();
        return true;
        
    } catch (...) {
        return false;
    }
}

void DrawESP(ImDrawList* draw_list, CachedGameData& cache, int screenWidth, int screenHeight) {
    if (!cache.entity_array || cache.entity_count <= 0) return;
    
    int processedEntities = 0;
    
    for (int i = 1; i < cache.entity_count; ++i) {
        // Limit entities processed per frame
        if (processedEntities >= Config::Performance::MaxEntitiesPerFrame)
            break;
            
        try {
            uintptr_t pawn = mem.read<uint64_t>(cache.entity_array + 0x20 + (i * 0x8));
            if (!pawn) continue;
            
            uint32_t prefab_id = mem.read<uint32_t>(pawn + GameOffsets::Entity::PrefabID);
            if (prefab_id != GameConstants::PlayerPrefabID) continue;
            
            // Get world position
            vec3 world = mem.read_chain<vec3>((PVOID)pawn, { 0x10, 0x30, 0x30, 0x8, 0x38, 0x90 });
            vec3 head = GetBonePosition(mem, pawn, BoneList::head);
            
            // World to screen
            vec2 worldpos = WorldToScreen(world, cache.view_matrix, screenWidth, screenHeight);
            vec2 headpos = WorldToScreen(head, cache.view_matrix, screenWidth, screenHeight);
            
            // Check if on screen
            if (headpos.x < 0 || headpos.y < 0 || headpos.x > screenWidth || headpos.y > screenHeight)
                continue;
            
            processedEntities++;
            
            // Draw player name
            if (Config::Features::DrawName) {
                draw_list->AddText(
                    ImVec2(headpos.x, headpos.y - 15), 
                    ImColor(Config::Visual::NameColor.r, Config::Visual::NameColor.g, 
                            Config::Visual::NameColor.b, Config::Visual::NameColor.a), 
                    "Player"
                );
            }
            
            // Draw skeleton
            if (Config::Features::DrawSkeleton) {
                for (const auto& [from, to] : SkeletonPairs) {
                    vec3 bone_from = GetBonePosition(mem, pawn, from);
                    vec3 bone_to = GetBonePosition(mem, pawn, to);
                    
                    vec2 screen_from = WorldToScreen(bone_from, cache.view_matrix, screenWidth, screenHeight);
                    vec2 screen_to = WorldToScreen(bone_to, cache.view_matrix, screenWidth, screenHeight);
                    
                    // Check if both points are valid
                    if (screen_from.x > 0 && screen_from.y > 0 && 
                        screen_to.x > 0 && screen_to.y > 0 &&
                        screen_from.x < screenWidth && screen_from.y < screenHeight &&
                        screen_to.x < screenWidth && screen_to.y < screenHeight) {
                        
                        draw_list->AddLine(
                            ImVec2(screen_from.x, screen_from.y),
                            ImVec2(screen_to.x, screen_to.y),
                            ImColor(Config::Visual::SkeletonColor.r, Config::Visual::SkeletonColor.g, 
                                    Config::Visual::SkeletonColor.b, Config::Visual::SkeletonColor.a),
                            Config::Visual::SkeletonThickness
                        );
                    }
                }
            }
            
        } catch (...) {
            continue;
        }
    }
}

bool WaitForGameFocus(DWORD processId) {
    std::wcout << L"[*] Waiting for game window focus...\n";
    
    while (true) {
        DWORD foregroundPID;
        GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPID);
        
        if (foregroundPID == processId) {
            std::wcout << L"[+] Game window is focused\n";
            return true;
        }
        
        Sleep(100);
    }
}

int main() {
    std::wcout << L"========================================\n";
    std::wcout << L"  Rust ESP - External Overlay\n";
    std::wcout << L"========================================\n\n";
    
    // Initialize memory
    std::wcout << L"[*] Setting up for RustClient.exe...\n";
    if (!mem.setup(L"RustClient.exe")) {
        std::wcout << L"[-] Setup failed. Is the game running?\n";
        std::wcout << L"[*] Press any key to exit...\n";
        std::cin.get();
        return 1;
    }
    
    auto cr3 = mem.GetCR3();
    std::wcout << L"[+] CR3: 0x" << std::hex << cr3 << std::dec << L"\n";
    
    auto game_base = mem.GetBase();
    std::wcout << L"[+] Base address: 0x" << std::hex << game_base << std::dec << L"\n";
    
    uintptr_t game_assembly = mem.get_module(L"GameAssembly.dll");
    if (!game_assembly) {
        std::wcout << L"[-] Failed to get GameAssembly.dll\n";
        std::wcout << L"[*] Press any key to exit...\n";
        std::cin.get();
        return 1;
    }
    std::wcout << L"[+] GameAssembly.dll: 0x" << std::hex << game_assembly << std::dec << L"\n\n";
    
    // Initialize overlay
    std::wcout << L"[*] Initializing overlay...\n";
    Overlay ovr;
    if (!ovr.Initialize()) {
        std::wcout << L"[-] Failed to initialize overlay\n";
        std::wcout << L"[*] Press any key to exit...\n";
        std::cin.get();
        return 1;
    }
    
    // Wait for game focus
    WaitForGameFocus(mem.Pid());
    
    // Initialize game data cache
    CachedGameData cache;
    cache.game_assembly = game_assembly;
    
    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    std::wcout << L"[+] ESP running! Press ALT+F4 on overlay to exit.\n\n";
    
    // Main render loop
    bool running = true;
    while (running) {
        // Process Windows messages
        if (PeekMessage(&ovr.dx9.message, ovr.window.hWnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&ovr.dx9.message);
            DispatchMessage(&ovr.dx9.message);
            
            if (ovr.dx9.message.message == WM_QUIT) {
                running = false;
                break;
            }
        }
        
        // Begin ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // Update game data
        if (UpdateGameData(cache)) {
            // Draw ESP
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            DrawESP(draw_list, cache, screenWidth, screenHeight);
        }
        
        // Render
        ImGui::EndFrame();
        
        ovr.dx9.device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        
        if (ovr.dx9.device->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            ovr.dx9.device->EndScene();
        }
        
        HRESULT result = ovr.dx9.device->Present(NULL, NULL, NULL, NULL);
        
        // Handle device loss
        if (result == D3DERR_DEVICELOST && 
            ovr.dx9.device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            ovr.dx9.device->Reset(&ovr.dx9.params);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
        
        // Small sleep to prevent 100% CPU usage
        Sleep(1);
    }
    
    // Cleanup
    std::wcout << L"\n[*] Shutting down...\n";
    ovr.Cleanup();
    std::wcout << L"[+] Cleanup complete\n";
    
    return 0;
}
