#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include <string>

class Kmem {
private:
    HANDLE hDriver;
    DWORD processId;
    uintptr_t baseAddress;
    uintptr_t cr3;

    DWORD GetProcessIdByName(const wchar_t* processName);
    uintptr_t GetModuleBase(const wchar_t* moduleName);

public:
    Kmem() : hDriver(NULL), processId(0), baseAddress(0), cr3(0) {}
    ~Kmem();

    bool setup(const wchar_t* processName);
    uintptr_t GetBase() { return baseAddress; }
    uintptr_t GetCR3() { return cr3; }
    uintptr_t get_module(const wchar_t* moduleName);
    DWORD Pid() { return processId; }

    template<typename T>
    T read(uintptr_t address) {
        T buffer{};
        if (!address) return buffer;

        SIZE_T bytesRead;
        HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, processId);
        if (hProcess) {
            ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), &bytesRead);
            CloseHandle(hProcess);
        }
        return buffer;
    }

    template<typename T>
    bool write(uintptr_t address, T value) {
        if (!address) return false;

        SIZE_T bytesWritten;
        HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processId);
        if (hProcess) {
            bool result = WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(T), &bytesWritten);
            CloseHandle(hProcess);
            return result && bytesWritten == sizeof(T);
        }
        return false;
    }

    template<typename T>
    T read_chain(PVOID base, std::vector<uintptr_t> offsets) {
        uintptr_t addr = (uintptr_t)base;
        for (size_t i = 0; i < offsets.size() - 1; i++) {
            addr = read<uintptr_t>(addr + offsets[i]);
            if (!addr) return T{};
        }
        return read<T>(addr + offsets[offsets.size() - 1]);
    }
};
