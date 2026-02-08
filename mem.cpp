#include "mem.h"
#include <iostream>

Kmem::~Kmem() {
    if (hDriver && hDriver != INVALID_HANDLE_VALUE) {
        CloseHandle(hDriver);
    }
}

DWORD Kmem::GetProcessIdByName(const wchar_t* processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry = { sizeof(PROCESSENTRY32W) };
        
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (_wcsicmp(processEntry.szExeFile, processName) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return processId;
}

uintptr_t Kmem::GetModuleBase(const wchar_t* moduleName) {
    uintptr_t moduleBase = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W moduleEntry = { sizeof(MODULEENTRY32W) };
        
        if (Module32FirstW(snapshot, &moduleEntry)) {
            do {
                if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
                    moduleBase = (uintptr_t)moduleEntry.modBaseAddr;
                    break;
                }
            } while (Module32NextW(snapshot, &moduleEntry));
        }
        CloseHandle(snapshot);
    }
    return moduleBase;
}

bool Kmem::setup(const wchar_t* processName) {
    processId = GetProcessIdByName(processName);
    if (!processId) {
        std::wcout << L"[-] Failed to find process: " << processName << L"\n";
        return false;
    }

    baseAddress = GetModuleBase(processName);
    if (!baseAddress) {
        std::wcout << L"[-] Failed to get base address\n";
        return false;
    }

    // For CR3, we'll use a placeholder since we're using ReadProcessMemory
    // In a real kernel driver implementation, you'd get the actual CR3
    cr3 = 0xDEADBEEF;
    
    std::wcout << L"[+] Process found: " << processName << L" (PID: " << processId << L")\n";
    return true;
}

uintptr_t Kmem::get_module(const wchar_t* moduleName) {
    return GetModuleBase(moduleName);
}
