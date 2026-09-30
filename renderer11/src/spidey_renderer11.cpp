#include "spidey_renderer11_api.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <unordered_map>

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

    ID3D11Texture2D* gUploadTexture = nullptr;
    ID3D11ShaderResourceView* gUploadSrv = nullptr;
    ID3D11VertexShader* gBlitVertexShader = nullptr;
    ID3D11PixelShader* gBlitPixelShader = nullptr;
    ID3D11SamplerState* gBlitSampler = nullptr;
    ID3D11RasterizerState* gBlitRasterizer = nullptr;
    unsigned long gUploadWidth = 0;
    unsigned long gUploadHeight = 0;

    struct GameTexture
    {
        ID3D11Texture2D* texture;
        ID3D11ShaderResourceView* srv;
        unsigned long width;
        unsigned long height;
        unsigned long sourceBitsPerPixel;
        unsigned long legacyHandle;
    };

    static const unsigned long kGameTextureCapacity = 1024;
    GameTexture gGameTextures[kGameTextureCapacity] = {};
    unsigned long gResidentTextureCount = 0;
    std::unordered_map<unsigned long, unsigned long> gLegacyHandleToTextureId;

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

    unsigned int CountTrailingZeroBits(unsigned long mask)
    {
        if (!mask)
            return 0;

        unsigned int shift = 0;
        while ((mask & 1UL) == 0)
        {
            ++shift;
            mask >>= 1;
        }
        return shift;
    }

    unsigned char ExpandMaskedComponent(
        unsigned long pixel,
        unsigned long mask,
        unsigned char defaultValue)
    {
        if (!mask)
            return defaultValue;

        const unsigned int shift = CountTrailingZeroBits(mask);
        const unsigned long maximum = mask >> shift;
        if (!maximum)
            return defaultValue;

        const unsigned long value = (pixel & mask) >> shift;
        return static_cast<unsigned char>(
            (value * 255UL + maximum / 2UL) / maximum);
    }

    void ReleaseGameTexture(unsigned long textureId)
    {
        if (textureId >= kGameTextureCapacity)
            return;

        GameTexture& entry = gGameTextures[textureId];
        const bool wasResident = entry.texture != nullptr || entry.srv != nullptr;

        if (entry.legacyHandle)
            gLegacyHandleToTextureId.erase(entry.legacyHandle);

        SafeRelease(entry.srv);
        SafeRelease(entry.texture);
        entry.width = 0;
        entry.height = 0;
        entry.sourceBitsPerPixel = 0;
        entry.legacyHandle = 0;

        if (wasResident && gResidentTextureCount)
            --gResidentTextureCount;
    }

    void ReleaseAllGameTexturesInternal()
    {
        for (unsigned long textureId = 0;
             textureId < kGameTextureCapacity;
             ++textureId)
        {
            ReleaseGameTexture(textureId);
        }

        gResidentTextureCount = 0;
        gLegacyHandleToTextureId.clear();
    }

    void ReleaseTargets()
    {
        if (gContext)
            gContext->OMSetRenderTargets(0, nullptr, nullptr);

        SafeRelease(gDepthStencilView);
        SafeRelease(gDepthTexture);
        SafeRelease(gRenderTargetView);
    }

    void ReleaseUploadTexture()
    {
        SafeRelease(gUploadSrv);
        SafeRelease(gUploadTexture);
        gUploadWidth = 0;
        gUploadHeight = 0;
    }

    void ReleaseBlitPipeline()
    {
        ReleaseUploadTexture();
        SafeRelease(gBlitRasterizer);
        SafeRelease(gBlitSampler);
        SafeRelease(gBlitPixelShader);
        SafeRelease(gBlitVertexShader);
    }

    void ReleaseDevice()
    {
        ReleaseTargets();
        ReleaseBlitPipeline();
        ReleaseAllGameTexturesInternal();
        SafeRelease(gSwapChain);
        SafeRelease(gContext);
        SafeRelease(gDevice);
        gWindow = nullptr;
        gWidth = 0;
        gHeight = 0;
    }

    bool CreateBlitPipeline()
    {
        if (!gDevice)
            return false;

        if (gBlitVertexShader && gBlitPixelShader && gBlitSampler && gBlitRasterizer)
            return true;

        static const char* kShaderSource =
            "Texture2D frameTexture : register(t0);\n"
            "SamplerState frameSampler : register(s0);\n"
            "struct VSOut { float4 position : SV_POSITION; float2 uv : TEXCOORD0; };\n"
            "VSOut VSMain(uint vertexId : SV_VertexID) {\n"
            "    float2 positions[3] = { float2(-1.0, -1.0), float2(-1.0, 3.0), float2(3.0, -1.0) };\n"
            "    float2 uvs[3] = { float2(0.0, 1.0), float2(0.0, -1.0), float2(2.0, 1.0) };\n"
            "    VSOut output;\n"
            "    output.position = float4(positions[vertexId], 0.0, 1.0);\n"
            "    output.uv = uvs[vertexId];\n"
            "    return output;\n"
            "}\n"
            "float4 PSMain(VSOut input) : SV_TARGET {\n"
            "    return frameTexture.Sample(frameSampler, input.uv);\n"
            "}\n";

        ID3DBlob* vertexBlob = nullptr;
        ID3DBlob* pixelBlob = nullptr;
        ID3DBlob* errors = nullptr;

        HRESULT hr = D3DCompile(
            kShaderSource,
            std::strlen(kShaderSource),
            "spidey_renderer11_blit",
            nullptr,
            nullptr,
            "VSMain",
            "vs_4_0",
            D3DCOMPILE_ENABLE_STRICTNESS,
            0,
            &vertexBlob,
            &errors);

        if (FAILED(hr))
        {
            Log(
                "blit_pipeline compile_vs failed hr=0x%08lX error=%s",
                static_cast<unsigned long>(hr),
                errors ? static_cast<const char*>(errors->GetBufferPointer()) : "none");
            SafeRelease(errors);
            SafeRelease(vertexBlob);
            return false;
        }

        SafeRelease(errors);

        hr = D3DCompile(
            kShaderSource,
            std::strlen(kShaderSource),
            "spidey_renderer11_blit",
            nullptr,
            nullptr,
            "PSMain",
            "ps_4_0",
            D3DCOMPILE_ENABLE_STRICTNESS,
            0,
            &pixelBlob,
            &errors);

        if (FAILED(hr))
        {
            Log(
                "blit_pipeline compile_ps failed hr=0x%08lX error=%s",
                static_cast<unsigned long>(hr),
                errors ? static_cast<const char*>(errors->GetBufferPointer()) : "none");
            SafeRelease(errors);
            SafeRelease(pixelBlob);
            SafeRelease(vertexBlob);
            return false;
        }

        SafeRelease(errors);

        hr = gDevice->CreateVertexShader(
            vertexBlob->GetBufferPointer(),
            vertexBlob->GetBufferSize(),
            nullptr,
            &gBlitVertexShader);

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreatePixelShader(
                pixelBlob->GetBufferPointer(),
                pixelBlob->GetBufferSize(),
                nullptr,
                &gBlitPixelShader);
        }

        SafeRelease(pixelBlob);
        SafeRelease(vertexBlob);

        if (FAILED(hr) || !gBlitVertexShader || !gBlitPixelShader)
        {
            Log("blit_pipeline create_shader failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseBlitPipeline();
            return false;
        }

        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MinLOD = 0.0f;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        hr = gDevice->CreateSamplerState(
            &samplerDesc,
            &gBlitSampler);

        if (FAILED(hr) || !gBlitSampler)
        {
            Log("blit_pipeline create_sampler failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseBlitPipeline();
            return false;
        }

        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.DepthClipEnable = TRUE;

        hr = gDevice->CreateRasterizerState(
            &rasterizerDesc,
            &gBlitRasterizer);

        if (FAILED(hr) || !gBlitRasterizer)
        {
            Log("blit_pipeline create_rasterizer failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseBlitPipeline();
            return false;
        }

        Log("blit_pipeline ready shader_model=4_0 filter=point cull=none");
        return true;
    }

    bool EnsureUploadTexture(unsigned long width, unsigned long height)
    {
        if (!gDevice || width == 0 || height == 0)
            return false;

        if (gUploadTexture && gUploadSrv &&
            gUploadWidth == width && gUploadHeight == height)
        {
            return true;
        }

        ReleaseUploadTexture();

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = gDevice->CreateTexture2D(
            &desc,
            nullptr,
            &gUploadTexture);

        if (FAILED(hr) || !gUploadTexture)
        {
            Log(
                "upload_texture create failed hr=0x%08lX size=%lux%lu",
                static_cast<unsigned long>(hr),
                width,
                height);
            ReleaseUploadTexture();
            return false;
        }

        hr = gDevice->CreateShaderResourceView(
            gUploadTexture,
            nullptr,
            &gUploadSrv);

        if (FAILED(hr) || !gUploadSrv)
        {
            Log(
                "upload_texture create_srv failed hr=0x%08lX size=%lux%lu",
                static_cast<unsigned long>(hr),
                width,
                height);
            ReleaseUploadTexture();
            return false;
        }

        gUploadWidth = width;
        gUploadHeight = height;

        Log("upload_texture ready width=%lu height=%lu format=BGRA8", width, height);
        return true;
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
int __cdecl SpideyRenderer11_PresentPixels(
    const void* pixels,
    unsigned long sourceWidth,
    unsigned long sourceHeight,
    long sourcePitch,
    int preserveAspect,
    int vsync)
{
    if (!pixels || !gSwapChain || !gContext || !gWindow ||
        sourceWidth == 0 || sourceHeight == 0 ||
        sourcePitch <= 0 ||
        static_cast<unsigned long>(sourcePitch) < sourceWidth * 4UL)
    {
        Log(
            "present_pixels rejected pixels=0x%p swap=0x%p hwnd=0x%p src=%lux%lu pitch=%ld",
            pixels,
            gSwapChain,
            gWindow,
            sourceWidth,
            sourceHeight,
            sourcePitch);
        return 0;
    }

    RECT client = {};
    if (!GetClientRect(gWindow, &client))
    {
        Log("present_pixels get_client_rect failed error=%lu", GetLastError());
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
                "present_pixels resize_failed target=%lux%lu",
                targetWidth,
                targetHeight);
            return 0;
        }
    }

    if (!CreateBlitPipeline() ||
        !EnsureUploadTexture(sourceWidth, sourceHeight))
    {
        return 0;
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = gContext->Map(
        gUploadTexture,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped);

    if (FAILED(hr) || !mapped.pData)
    {
        Log("present_pixels map failed hr=0x%08lX", static_cast<unsigned long>(hr));
        return 0;
    }

    const unsigned char* source =
        static_cast<const unsigned char*>(pixels);
    unsigned char* destination =
        static_cast<unsigned char*>(mapped.pData);
    const size_t rowBytes =
        static_cast<size_t>(sourceWidth) * 4U;

    for (unsigned long y = 0; y < sourceHeight; ++y)
    {
        std::memcpy(
            destination + static_cast<size_t>(y) * mapped.RowPitch,
            source + static_cast<size_t>(y) * static_cast<size_t>(sourcePitch),
            rowBytes);
    }

    gContext->Unmap(gUploadTexture, 0);

    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    gContext->OMSetRenderTargets(
        1,
        &gRenderTargetView,
        nullptr);
    gContext->ClearRenderTargetView(
        gRenderTargetView,
        clearColor);

    unsigned long presentWidth = targetWidth;
    unsigned long presentHeight = targetHeight;
    unsigned long presentX = 0;
    unsigned long presentY = 0;

    if (preserveAspect)
    {
        const unsigned long long srcWide =
            static_cast<unsigned long long>(sourceWidth) * targetHeight;
        const unsigned long long dstWide =
            static_cast<unsigned long long>(targetWidth) * sourceHeight;

        if (srcWide > dstWide)
        {
            presentHeight =
                static_cast<unsigned long>(
                    static_cast<unsigned long long>(targetWidth) *
                    sourceHeight / sourceWidth);
            presentY = (targetHeight - presentHeight) / 2;
        }
        else if (srcWide < dstWide)
        {
            presentWidth =
                static_cast<unsigned long>(
                    static_cast<unsigned long long>(targetHeight) *
                    sourceWidth / sourceHeight);
            presentX = (targetWidth - presentWidth) / 2;
        }
    }

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = static_cast<float>(presentX);
    viewport.TopLeftY = static_cast<float>(presentY);
    viewport.Width = static_cast<float>(presentWidth);
    viewport.Height = static_cast<float>(presentHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    gContext->RSSetViewports(1, &viewport);
    gContext->RSSetState(gBlitRasterizer);

    gContext->IASetInputLayout(nullptr);
    gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gContext->VSSetShader(gBlitVertexShader, nullptr, 0);
    gContext->PSSetShader(gBlitPixelShader, nullptr, 0);
    gContext->PSSetShaderResources(0, 1, &gUploadSrv);
    gContext->PSSetSamplers(0, 1, &gBlitSampler);
    gContext->Draw(3, 0);

    ID3D11ShaderResourceView* nullSrv = nullptr;
    gContext->PSSetShaderResources(0, 1, &nullSrv);

    hr = gSwapChain->Present(vsync ? 1 : 0, 0);
    if (FAILED(hr))
    {
        Log("present_pixels present_failed hr=0x%08lX", static_cast<unsigned long>(hr));
        return 0;
    }

    static unsigned long frame = 0;
    ++frame;
    if (frame <= 5 || (frame % 120) == 0)
    {
        Log(
            "present_pixels frame=%lu src=%lux%lu pitch=%ld dst=%lux%lu rect=%lu,%lu,%lux%lu aspect=%d vsync=%d",
            frame,
            sourceWidth,
            sourceHeight,
            sourcePitch,
            targetWidth,
            targetHeight,
            presentX,
            presentY,
            presentWidth,
            presentHeight,
            preserveAspect ? 1 : 0,
            vsync ? 1 : 0);
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
int __cdecl SpideyRenderer11_UpdateTexture(
    unsigned long textureId,
    const void* pixels,
    unsigned long width,
    unsigned long height,
    long pitch,
    unsigned long bitsPerPixel,
    unsigned long redMask,
    unsigned long greenMask,
    unsigned long blueMask,
    unsigned long alphaMask)
{
    if (!gDevice ||
        textureId >= kGameTextureCapacity ||
        !pixels ||
        width == 0 ||
        height == 0 ||
        pitch == 0 ||
        (bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32))
    {
        Log(
            "texture_update rejected id=%lu pixels=0x%p size=%lux%lu pitch=%ld bpp=%lu device=0x%p",
            textureId,
            pixels,
            width,
            height,
            pitch,
            bitsPerPixel,
            gDevice);
        return 0;
    }

    const unsigned long bytesPerPixel = bitsPerPixel / 8UL;
    if (bytesPerPixel == 0)
        return 0;

    const unsigned long absolutePitch =
        pitch < 0 ? static_cast<unsigned long>(-pitch) : static_cast<unsigned long>(pitch);

    if (absolutePitch < width * bytesPerPixel)
    {
        Log(
            "texture_update bad_pitch id=%lu pitch=%ld minimum=%lu",
            textureId,
            pitch,
            width * bytesPerPixel);
        return 0;
    }

    std::vector<unsigned char> converted;
    try
    {
        converted.resize(
            static_cast<size_t>(width) *
            static_cast<size_t>(height) *
            4U);
    }
    catch (...)
    {
        Log("texture_update allocation_failed id=%lu size=%lux%lu", textureId, width, height);
        return 0;
    }

    const unsigned char* sourceBase =
        static_cast<const unsigned char*>(pixels);

    for (unsigned long y = 0; y < height; ++y)
    {
        const long sourceOffset =
            static_cast<long>(y) * pitch;
        const unsigned char* sourceRow =
            sourceBase + sourceOffset;
        unsigned char* destinationRow =
            converted.data() +
            static_cast<size_t>(y) *
            static_cast<size_t>(width) *
            4U;

        for (unsigned long x = 0; x < width; ++x)
        {
            const unsigned char* sourcePixel =
                sourceRow +
                static_cast<size_t>(x) *
                bytesPerPixel;

            unsigned long raw = 0;
            if (bitsPerPixel == 16)
            {
                raw =
                    static_cast<unsigned long>(sourcePixel[0]) |
                    (static_cast<unsigned long>(sourcePixel[1]) << 8);
            }
            else if (bitsPerPixel == 24)
            {
                raw =
                    static_cast<unsigned long>(sourcePixel[0]) |
                    (static_cast<unsigned long>(sourcePixel[1]) << 8) |
                    (static_cast<unsigned long>(sourcePixel[2]) << 16);
            }
            else
            {
                raw =
                    static_cast<unsigned long>(sourcePixel[0]) |
                    (static_cast<unsigned long>(sourcePixel[1]) << 8) |
                    (static_cast<unsigned long>(sourcePixel[2]) << 16) |
                    (static_cast<unsigned long>(sourcePixel[3]) << 24);
            }

            const size_t destinationOffset =
                static_cast<size_t>(x) * 4U;

            destinationRow[destinationOffset + 0] =
                ExpandMaskedComponent(raw, blueMask, 0);
            destinationRow[destinationOffset + 1] =
                ExpandMaskedComponent(raw, greenMask, 0);
            destinationRow[destinationOffset + 2] =
                ExpandMaskedComponent(raw, redMask, 0);
            destinationRow[destinationOffset + 3] =
                ExpandMaskedComponent(raw, alphaMask, 255);
        }
    }

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = converted.data();
    initialData.SysMemPitch = width * 4UL;

    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = gDevice->CreateTexture2D(
        &textureDesc,
        &initialData,
        &texture);

    if (FAILED(hr) || !texture)
    {
        Log(
            "texture_update create_texture_failed id=%lu hr=0x%08lX size=%lux%lu bpp=%lu",
            textureId,
            static_cast<unsigned long>(hr),
            width,
            height,
            bitsPerPixel);
        SafeRelease(texture);
        return 0;
    }

    ID3D11ShaderResourceView* srv = nullptr;
    hr = gDevice->CreateShaderResourceView(
        texture,
        nullptr,
        &srv);

    if (FAILED(hr) || !srv)
    {
        Log(
            "texture_update create_srv_failed id=%lu hr=0x%08lX",
            textureId,
            static_cast<unsigned long>(hr));
        SafeRelease(srv);
        SafeRelease(texture);
        return 0;
    }

    ReleaseGameTexture(textureId);

    GameTexture& entry = gGameTextures[textureId];
    entry.texture = texture;
    entry.srv = srv;
    entry.width = width;
    entry.height = height;
    entry.sourceBitsPerPixel = bitsPerPixel;
    entry.legacyHandle = 0;
    ++gResidentTextureCount;

    Log(
        "texture_update id=%lu size=%lux%lu src_bpp=%lu masks=%08lX,%08lX,%08lX,%08lX resident=%lu",
        textureId,
        width,
        height,
        bitsPerPixel,
        redMask,
        greenMask,
        blueMask,
        alphaMask,
        gResidentTextureCount);

    return 1;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_AssociateTextureHandle(
    unsigned long textureId,
    unsigned long legacyHandle)
{
    if (textureId >= kGameTextureCapacity ||
        !legacyHandle ||
        !gGameTextures[textureId].srv)
    {
        Log(
            "texture_handle rejected id=%lu handle=0x%08lX resident=%d",
            textureId,
            legacyHandle,
            textureId < kGameTextureCapacity && gGameTextures[textureId].srv ? 1 : 0);
        return 0;
    }

    GameTexture& entry = gGameTextures[textureId];

    if (entry.legacyHandle)
        gLegacyHandleToTextureId.erase(entry.legacyHandle);

    entry.legacyHandle = legacyHandle;
    gLegacyHandleToTextureId[legacyHandle] = textureId;

    Log(
        "texture_handle id=%lu handle=0x%08lX",
        textureId,
        legacyHandle);

    return 1;
}

extern "C" __declspec(dllexport)
long __cdecl SpideyRenderer11_ResolveTextureHandle(
    unsigned long legacyHandle)
{
    if (!legacyHandle)
        return -1;

    const auto found =
        gLegacyHandleToTextureId.find(legacyHandle);

    if (found == gLegacyHandleToTextureId.end())
        return -1;

    return static_cast<long>(found->second);
}

extern "C" __declspec(dllexport)
void __cdecl SpideyRenderer11_ReleaseTexture(
    unsigned long textureId)
{
    if (textureId >= kGameTextureCapacity)
        return;

    const bool wasResident =
        gGameTextures[textureId].texture != nullptr ||
        gGameTextures[textureId].srv != nullptr;

    ReleaseGameTexture(textureId);

    if (wasResident)
    {
        Log(
            "texture_release id=%lu resident=%lu",
            textureId,
            gResidentTextureCount);
    }
}

extern "C" __declspec(dllexport)
void __cdecl SpideyRenderer11_ReleaseAllTextures(void)
{
    const unsigned long before = gResidentTextureCount;
    ReleaseAllGameTexturesInternal();
    Log("texture_release_all before=%lu resident=0", before);
}

extern "C" __declspec(dllexport)
unsigned long __cdecl SpideyRenderer11_GetResidentTextureCount(void)
{
    return gResidentTextureCount;
}

extern "C" __declspec(dllexport)
void __cdecl SpideyRenderer11_Shutdown(void)
{
    if (gContext)
        gContext->ClearState();

    ReleaseDevice();
}
