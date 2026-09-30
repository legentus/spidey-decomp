#include "spidey_renderer11_api.h"

#include <d3d11.h>
#include <dxgi.h>

#include <cstdio>
#include <cstdarg>

namespace
{
    ID3D11Device* gDevice = nullptr;
    ID3D11DeviceContext* gContext = nullptr;
    IDXGISwapChain* gSwapChain = nullptr;
    ID3D11RenderTargetView* gRenderTargetView = nullptr;
    ID3D11Texture2D* gDepthTexture = nullptr;
    ID3D11DepthStencilView* gDepthStencilView = nullptr;
    D3D_FEATURE_LEVEL gFeatureLevel = D3D_FEATURE_LEVEL_9_1;
    HWND gWindow = nullptr;
    unsigned long gWidth = 0;
    unsigned long gHeight = 0;

    void Log(const char* format, ...)
    {
        FILE* file = std::fopen("spidey-renderer11.log", "a");
        if (!file)
            return;

        va_list args;
        va_start(args, format);
        std::vfprintf(file, format, args);
        va_end(args);

        std::fputc('\n', file);
        std::fclose(file);
    }

    template <typename T>
    void SafeRelease(T*& object)
    {
        if (object)
        {
            object->Release();
            object = nullptr;
        }
    }

    void ReleaseTargets()
    {
        if (gContext)
            gContext->OMSetRenderTargets(0, nullptr, nullptr);

        SafeRelease(gDepthStencilView);
        SafeRelease(gDepthTexture);
        SafeRelease(gRenderTargetView);
    }

    void ReleaseDevice()
    {
        ReleaseTargets();
        SafeRelease(gSwapChain);
        SafeRelease(gContext);
        SafeRelease(gDevice);
        gWindow = nullptr;
        gWidth = 0;
        gHeight = 0;
    }

    bool CreateTargets(unsigned long width, unsigned long height)
    {
        if (!gDevice || !gContext || !gSwapChain || width == 0 || height == 0)
            return false;

        ID3D11Texture2D* backBuffer = nullptr;
        HRESULT hr = gSwapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&backBuffer));

        if (FAILED(hr) || !backBuffer)
        {
            Log("targets get_backbuffer failed hr=0x%08lX", static_cast<unsigned long>(hr));
            SafeRelease(backBuffer);
            return false;
        }

        hr = gDevice->CreateRenderTargetView(
            backBuffer,
            nullptr,
            &gRenderTargetView);
        SafeRelease(backBuffer);

        if (FAILED(hr) || !gRenderTargetView)
        {
            Log("targets create_rtv failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseTargets();
            return false;
        }

        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        hr = gDevice->CreateTexture2D(
            &depthDesc,
            nullptr,
            &gDepthTexture);

        if (FAILED(hr) || !gDepthTexture)
        {
            Log("targets create_depth failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseTargets();
            return false;
        }

        hr = gDevice->CreateDepthStencilView(
            gDepthTexture,
            nullptr,
            &gDepthStencilView);

        if (FAILED(hr) || !gDepthStencilView)
        {
            Log("targets create_dsv failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseTargets();
            return false;
        }

        gContext->OMSetRenderTargets(
            1,
            &gRenderTargetView,
            gDepthStencilView);

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        gContext->RSSetViewports(1, &viewport);

        gWidth = width;
        gHeight = height;

        Log("targets ready width=%lu height=%lu", width, height);
        return true;
    }

    void LogAdapter()
    {
        if (!gDevice)
            return;

        IDXGIDevice* dxgiDevice = nullptr;
        IDXGIAdapter* adapter = nullptr;

        HRESULT hr = gDevice->QueryInterface(
            __uuidof(IDXGIDevice),
            reinterpret_cast<void**>(&dxgiDevice));

        if (SUCCEEDED(hr) && dxgiDevice)
            hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr) && adapter)
        {
            DXGI_ADAPTER_DESC desc = {};
            if (SUCCEEDED(adapter->GetDesc(&desc)))
            {
                char description[256] = {};
                WideCharToMultiByte(
                    CP_UTF8,
                    0,
                    desc.Description,
                    -1,
                    description,
                    static_cast<int>(sizeof(description)),
                    nullptr,
                    nullptr);

                Log(
                    "adapter name=%s vendor=0x%04X device=0x%04X dedicated_video=%llu",
                    description,
                    desc.VendorId,
                    desc.DeviceId,
                    static_cast<unsigned long long>(desc.DedicatedVideoMemory));
            }
        }

        SafeRelease(adapter);
        SafeRelease(dxgiDevice);
    }
}

extern "C" __declspec(dllexport)
unsigned long __cdecl SpideyRenderer11_GetAbiVersion(void)
{
    return SPIDEY_RENDERER11_ABI_VERSION;
}

extern "C" __declspec(dllexport)
const char* __cdecl SpideyRenderer11_GetBackendName(void)
{
    return "Direct3D 11";
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_Probe(void)
{
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_9_1;

    const D3D_FEATURE_LEVEL requested[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3
    };

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        requested,
        static_cast<UINT>(sizeof(requested) / sizeof(requested[0])),
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &context);

    // Windows 7-era runtimes can reject a list containing 11_1 with
    // E_INVALIDARG. Retry without 11_1 so the bridge remains broadly usable.
    if (hr == E_INVALIDARG)
    {
        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            requested + 1,
            static_cast<UINT>((sizeof(requested) / sizeof(requested[0])) - 1),
            D3D11_SDK_VERSION,
            &device,
            &featureLevel,
            &context);
    }

    Log(
        "probe hr=0x%08lX feature_level=0x%04X",
        static_cast<unsigned long>(hr),
        static_cast<unsigned int>(featureLevel));

    SafeRelease(context);
    SafeRelease(device);

    return SUCCEEDED(hr) ? 1 : 0;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_Initialize(
    HWND hwnd,
    unsigned long width,
    unsigned long height)
{
    SpideyRenderer11_Shutdown();

    if (!hwnd || width == 0 || height == 0)
    {
        Log("initialize rejected hwnd=0x%p width=%lu height=%lu", hwnd, width, height);
        return 0;
    }

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferDesc.Width = width;
    swapDesc.BufferDesc.Height = height;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = 2;
    swapDesc.OutputWindow = hwnd;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;

    const D3D_FEATURE_LEVEL requested[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        requested,
        static_cast<UINT>(sizeof(requested) / sizeof(requested[0])),
        D3D11_SDK_VERSION,
        &swapDesc,
        &gSwapChain,
        &gDevice,
        &gFeatureLevel,
        &gContext);

    if (hr == E_INVALIDARG)
    {
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            requested + 1,
            static_cast<UINT>((sizeof(requested) / sizeof(requested[0])) - 1),
            D3D11_SDK_VERSION,
            &swapDesc,
            &gSwapChain,
            &gDevice,
            &gFeatureLevel,
            &gContext);
    }

    if (FAILED(hr))
    {
        Log(
            "initialize create_device_swapchain failed hr=0x%08lX width=%lu height=%lu",
            static_cast<unsigned long>(hr),
            width,
            height);
        ReleaseDevice();
        return 0;
    }

    gWindow = hwnd;

    Log(
        "initialize device_ready hwnd=0x%p width=%lu height=%lu feature_level=0x%04X",
        hwnd,
        width,
        height,
        static_cast<unsigned int>(gFeatureLevel));

    LogAdapter();

    if (!CreateTargets(width, height))
    {
        ReleaseDevice();
        return 0;
    }

    return 1;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_Resize(
    unsigned long width,
    unsigned long height)
{
    if (!gSwapChain || width == 0 || height == 0)
        return 0;

    ReleaseTargets();

    HRESULT hr = gSwapChain->ResizeBuffers(
        0,
        width,
        height,
        DXGI_FORMAT_UNKNOWN,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE);

    if (FAILED(hr))
    {
        Log(
            "resize failed hr=0x%08lX width=%lu height=%lu",
            static_cast<unsigned long>(hr),
            width,
            height);
        return 0;
    }

    return CreateTargets(width, height) ? 1 : 0;
}

extern "C" __declspec(dllexport)
void __cdecl SpideyRenderer11_BeginFrame(
    float r,
    float g,
    float b,
    float a)
{
    if (!gContext || !gRenderTargetView)
        return;

    const float clearColor[4] = { r, g, b, a };

    gContext->OMSetRenderTargets(
        1,
        &gRenderTargetView,
        gDepthStencilView);
    gContext->ClearRenderTargetView(
        gRenderTargetView,
        clearColor);

    if (gDepthStencilView)
    {
        gContext->ClearDepthStencilView(
            gDepthStencilView,
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f,
            0);
    }
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_Present(int vsync)
{
    if (!gSwapChain)
        return 0;

    HRESULT hr = gSwapChain->Present(vsync ? 1 : 0, 0);
    if (FAILED(hr))
    {
        Log("present failed hr=0x%08lX", static_cast<unsigned long>(hr));
        return 0;
    }

    return 1;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_PresentHdc(
    HDC source,
    unsigned long sourceWidth,
    unsigned long sourceHeight,
    int preserveAspect,
    int vsync)
{
    if (!source || !gSwapChain || !gContext || !gWindow ||
        sourceWidth == 0 || sourceHeight == 0)
    {
        Log(
            "present_hdc rejected source=0x%p swap=0x%p hwnd=0x%p src=%lux%lu",
            source,
            gSwapChain,
            gWindow,
            sourceWidth,
            sourceHeight);
        return 0;
    }

    RECT client = {};
    if (!GetClientRect(gWindow, &client))
    {
        Log("present_hdc get_client_rect failed error=%lu", GetLastError());
        return 0;
    }

    const unsigned long targetWidth =
        static_cast<unsigned long>(client.right - client.left);
    const unsigned long targetHeight =
        static_cast<unsigned long>(client.bottom - client.top);

    if (targetWidth == 0 || targetHeight == 0)
        return 0;

    if (targetWidth != gWidth || targetHeight != gHeight)
    {
        if (!SpideyRenderer11_Resize(targetWidth, targetHeight))
        {
            Log(
                "present_hdc resize_failed target=%lux%lu",
                targetWidth,
                targetHeight);
            return 0;
        }
    }

    if (gContext)
    {
        gContext->OMSetRenderTargets(0, nullptr, nullptr);
        gContext->Flush();
    }

    IDXGISurface1* surface = nullptr;
    HRESULT hr = gSwapChain->GetBuffer(
        0,
        __uuidof(IDXGISurface1),
        reinterpret_cast<void**>(&surface));

    if (FAILED(hr) || !surface)
    {
        Log("present_hdc get_surface failed hr=0x%08lX", static_cast<unsigned long>(hr));
        SafeRelease(surface);
        return 0;
    }

    HDC target = nullptr;
    hr = surface->GetDC(TRUE, &target);
    if (FAILED(hr) || !target)
    {
        Log("present_hdc get_dc failed hr=0x%08lX", static_cast<unsigned long>(hr));
        SafeRelease(surface);
        return 0;
    }

    int x = 0;
    int y = 0;
    int width = static_cast<int>(targetWidth);
    int height = static_cast<int>(targetHeight);

    if (preserveAspect)
    {
        const unsigned long long srcWide =
            static_cast<unsigned long long>(sourceWidth) * targetHeight;
        const unsigned long long dstWide =
            static_cast<unsigned long long>(targetWidth) * sourceHeight;

        if (srcWide > dstWide)
        {
            height = static_cast<int>(
                static_cast<unsigned long long>(targetWidth) *
                sourceHeight / sourceWidth);
            y = (static_cast<int>(targetHeight) - height) / 2;
        }
        else if (srcWide < dstWide)
        {
            width = static_cast<int>(
                static_cast<unsigned long long>(targetHeight) *
                sourceWidth / sourceHeight);
            x = (static_cast<int>(targetWidth) - width) / 2;
        }
    }

    RECT full = { 0, 0, static_cast<LONG>(targetWidth), static_cast<LONG>(targetHeight) };
    FillRect(target, &full, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    SetStretchBltMode(target, COLORONCOLOR);

    BOOL copied = StretchBlt(
        target,
        x,
        y,
        width,
        height,
        source,
        0,
        0,
        static_cast<int>(sourceWidth),
        static_cast<int>(sourceHeight),
        SRCCOPY);

    DWORD copyError = copied ? 0 : GetLastError();
    GdiFlush();

    HRESULT releaseHr = surface->ReleaseDC(nullptr);
    SafeRelease(surface);

    if (!copied || FAILED(releaseHr))
    {
        Log(
            "present_hdc copy_failed copied=%d error=%lu release_hr=0x%08lX src=%lux%lu dst=%lux%lu rect=%d,%d,%dx%d",
            copied ? 1 : 0,
            copyError,
            static_cast<unsigned long>(releaseHr),
            sourceWidth,
            sourceHeight,
            targetWidth,
            targetHeight,
            x,
            y,
            width,
            height);
        return 0;
    }

    hr = gSwapChain->Present(vsync ? 1 : 0, 0);
    if (FAILED(hr))
    {
        Log("present_hdc present_failed hr=0x%08lX", static_cast<unsigned long>(hr));
        return 0;
    }

    static unsigned long frame = 0;
    ++frame;
    if (frame <= 5 || (frame % 120) == 0)
    {
        Log(
            "present_hdc frame=%lu src=%lux%lu dst=%lux%lu rect=%d,%d,%dx%d aspect=%d vsync=%d",
            frame,
            sourceWidth,
            sourceHeight,
            targetWidth,
            targetHeight,
            x,
            y,
            width,
            height,
            preserveAspect ? 1 : 0,
            vsync ? 1 : 0);
    }

    return 1;
}

extern "C" __declspec(dllexport)
void __cdecl SpideyRenderer11_Shutdown(void)
{
    if (gContext)
        gContext->ClearState();

    ReleaseDevice();
}
