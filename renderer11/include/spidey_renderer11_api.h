#pragma once

#ifdef _WIN32
#include <windows.h>
#else
typedef void* HWND;
typedef void* HDC;
#endif

#define SPIDEY_RENDERER11_ABI_VERSION 7UL

typedef struct SpideyRenderer11ShadowVertex
{
    float x;
    float y;
    float z;
    float rhw;
    unsigned long diffuse;
    float u;
    float v;
} SpideyRenderer11ShadowVertex;

typedef struct SpideyRenderer11ShadowState
{
    unsigned long textureHandle;
    unsigned long viewportX;
    unsigned long viewportY;
    unsigned long viewportWidth;
    unsigned long viewportHeight;
    float viewportMinZ;
    float viewportMaxZ;
    unsigned long zEnable;
    unsigned long zWrite;
    unsigned long zFunc;
    unsigned long alphaBlendEnable;
    unsigned long srcBlend;
    unsigned long dstBlend;
    unsigned long alphaTestEnable;
    unsigned long alphaRef;
    unsigned long alphaFunc;
    unsigned long fogEnable;
    unsigned long fogColor;
    unsigned long colorOp;
    unsigned long colorArg1;
    unsigned long colorArg2;
    unsigned long alphaOp;
    unsigned long alphaArg1;
    unsigned long alphaArg2;
    unsigned long addressU;
    unsigned long addressV;
    unsigned long magFilter;
    unsigned long minFilter;
    unsigned long drawClass;
} SpideyRenderer11ShadowState;

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
__declspec(dllexport) int __cdecl SpideyRenderer11_PresentPixels(
    const void* pixels,
    unsigned long sourceWidth,
    unsigned long sourceHeight,
    long sourcePitch,
    int preserveAspect,
    int vsync);
__declspec(dllexport) int __cdecl SpideyRenderer11_PresentHdc(
    HDC source,
    unsigned long sourceWidth,
    unsigned long sourceHeight,
    int preserveAspect,
    int vsync);
__declspec(dllexport) int __cdecl SpideyRenderer11_UpdateTexture(
    unsigned long textureId,
    const void* pixels,
    unsigned long width,
    unsigned long height,
    long pitch,
    unsigned long bitsPerPixel,
    unsigned long redMask,
    unsigned long greenMask,
    unsigned long blueMask,
    unsigned long alphaMask);
__declspec(dllexport) int __cdecl SpideyRenderer11_AssociateTextureHandle(
    unsigned long textureId,
    unsigned long legacyHandle);
__declspec(dllexport) long __cdecl SpideyRenderer11_ResolveTextureHandle(
    unsigned long legacyHandle);
__declspec(dllexport) long __cdecl SpideyRenderer11_UpdateTransientTexture(
    unsigned long legacyHandle,
    const void* pixels,
    unsigned long width,
    unsigned long height,
    long pitch,
    unsigned long bitsPerPixel,
    unsigned long redMask,
    unsigned long greenMask,
    unsigned long blueMask,
    unsigned long alphaMask);
__declspec(dllexport) void __cdecl SpideyRenderer11_ShadowSetClear(
    unsigned long clearFlags,
    unsigned long clearColor,
    float clearDepth,
    unsigned long clearStencil);
__declspec(dllexport) int __cdecl SpideyRenderer11_ShadowSubmitTriangleFan(
    const SpideyRenderer11ShadowVertex* vertices,
    unsigned long vertexCount,
    const SpideyRenderer11ShadowState* state);
__declspec(dllexport) int __cdecl SpideyRenderer11_ShadowEndFrame(
    unsigned long frame,
    unsigned long sceneWidth,
    unsigned long sceneHeight);
__declspec(dllexport) void __cdecl SpideyRenderer11_ShadowSetContinuous(
    int enabled);
__declspec(dllexport) int __cdecl SpideyRenderer11_PresentShadow(
    int preserveAspect,
    int vsync);
__declspec(dllexport) void __cdecl SpideyRenderer11_ReleaseTexture(
    unsigned long textureId);
__declspec(dllexport) void __cdecl SpideyRenderer11_ReleaseAllTextures(void);
__declspec(dllexport) unsigned long __cdecl SpideyRenderer11_GetResidentTextureCount(void);
__declspec(dllexport) void __cdecl SpideyRenderer11_Shutdown(void);

#ifdef __cplusplus
}
#endif
