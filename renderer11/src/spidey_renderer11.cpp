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
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = 2;
    swapDesc.OutputWindow = hwnd;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

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
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

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
void __cdecl SpideyRenderer11_Shutdown(void)
{
    if (gContext)
        gContext->ClearState();

    ReleaseDevice();
}
