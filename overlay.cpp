#include "overlay.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            if (msg == WM_SIZE)
                return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool Overlay::Initialize() {
    // Create window class
    window.wc.cbSize = sizeof(WNDCLASSEXW);
    window.wc.style = CS_CLASSDC;
    window.wc.lpfnWndProc = WndProc;
    window.wc.cbClsExtra = 0;
    window.wc.cbWndExtra = 0;
    window.wc.hInstance = GetModuleHandle(NULL);
    window.wc.hIcon = NULL;
    window.wc.hCursor = NULL;
    window.wc.hbrBackground = NULL;
    window.wc.lpszMenuName = NULL;
    window.wc.lpszClassName = L"RustESP";
    window.wc.hIconSm = NULL;

    if (!RegisterClassExW(&window.wc)) {
        std::wcout << L"[-] Failed to register window class\n";
        return false;
    }

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create overlay window
    window.hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        window.wc.lpszClassName,
        L"Rust ESP",
        WS_POPUP,
        0, 0, screenWidth, screenHeight,
        NULL, NULL, window.wc.hInstance, NULL
    );

    if (!window.hWnd) {
        std::wcout << L"[-] Failed to create window\n";
        return false;
    }

    // Set window attributes
    SetLayeredWindowAttributes(window.hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    
    MARGINS margin = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(window.hWnd, &margin);

    ShowWindow(window.hWnd, SW_SHOW);
    UpdateWindow(window.hWnd);

    // Initialize DirectX 9
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &dx9.object))) {
        std::wcout << L"[-] Failed to create Direct3D9Ex object\n";
        return false;
    }

    ZeroMemory(&dx9.params, sizeof(dx9.params));
    dx9.params.Windowed = TRUE;
    dx9.params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    dx9.params.BackBufferFormat = D3DFMT_A8R8G8B8;
    dx9.params.EnableAutoDepthStencil = TRUE;
    dx9.params.AutoDepthStencilFormat = D3DFMT_D16;
    dx9.params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    dx9.params.BackBufferWidth = screenWidth;
    dx9.params.BackBufferHeight = screenHeight;

    if (FAILED(dx9.object->CreateDeviceEx(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window.hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &dx9.params,
        NULL,
        &dx9.device))) {
        std::wcout << L"[-] Failed to create Direct3D9 device\n";
        return false;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(window.hWnd)) {
        std::wcout << L"[-] Failed to initialize ImGui Win32\n";
        return false;
    }

    if (!ImGui_ImplDX9_Init(dx9.device)) {
        std::wcout << L"[-] Failed to initialize ImGui DX9\n";
        return false;
    }

    std::wcout << L"[+] Overlay initialized successfully\n";
    return true;
}

void Overlay::Cleanup() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (dx9.device) {
        dx9.device->Release();
        dx9.device = nullptr;
    }

    if (dx9.object) {
        dx9.object->Release();
        dx9.object = nullptr;
    }

    if (window.hWnd) {
        DestroyWindow(window.hWnd);
        window.hWnd = nullptr;
    }

    UnregisterClassW(window.wc.lpszClassName, window.wc.hInstance);
}
