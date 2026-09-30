#pragma once

#ifdef _WIN32
#include <windows.h>
#else
typedef void* HWND;
#endif

#define SPIDEY_RENDERER11_ABI_VERSION 2UL

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) unsigned long __cdecl SpideyRenderer11_GetAbiVersion(void);
__declspec(dllexport) const char* __cdecl SpideyRenderer11_GetBackendName(void);
__declspec(dllexport) int __cdecl SpideyRenderer11_Probe(void);
__declspec(dllexport) int __cdecl SpideyRenderer11_Initialize(
    HWND hwnd,
    unsigned long width,
    unsigned long height);
__declspec(dllexport) int __cdecl SpideyRenderer11_Resize(
    unsigned long width,
    unsigned long height);
__declspec(dllexport) void __cdecl SpideyRenderer11_BeginFrame(
    float r,
    float g,
    float b,
    float a);
__declspec(dllexport) int __cdecl SpideyRenderer11_Present(
    int vsync);
__declspec(dllexport) int __cdecl SpideyRenderer11_PresentHdc(
    HDC source,
    unsigned long sourceWidth,
    unsigned long sourceHeight,
    int preserveAspect,
    int vsync);
__declspec(dllexport) void __cdecl SpideyRenderer11_Shutdown(void);

#ifdef __cplusplus
}
#endif
