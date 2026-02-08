#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <dwmapi.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

struct D3D9 {
    IDirect3D9Ex* object = nullptr;
    IDirect3DDevice9Ex* device = nullptr;
    D3DPRESENT_PARAMETERS params = {};
    MSG message = {};
};

struct Window {
    HWND hWnd = nullptr;
    WNDCLASSEXW wc = {};
};

class Overlay {
public:
    D3D9 dx9;
    Window window;
    
    bool Initialize();
    void Cleanup();
};
