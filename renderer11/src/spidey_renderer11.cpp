#include "spidey_renderer11_api.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <cstdint>

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

    static const unsigned long kPersistentTextureCapacity = 1024;
    static const unsigned long kGameTextureCapacity = 2048;
    GameTexture gGameTextures[kGameTextureCapacity] = {};
    unsigned long gResidentTextureCount = 0;
    std::unordered_map<unsigned long, unsigned long> gLegacyHandleToTextureId;

    struct ShadowGpuVertex
    {
        float x;
        float y;
        float z;
        float w;
        unsigned long diffuse;
        float u;
        float v;
    };

    struct ShadowCommand
    {
        unsigned long firstVertex;
        unsigned long vertexCount;
        long textureId;
        SpideyRenderer11ShadowState state;
    };

    ID3D11Texture2D* gShadowColorTexture = nullptr;
    ID3D11RenderTargetView* gShadowRenderTargetView = nullptr;
    ID3D11ShaderResourceView* gShadowColorSrv = nullptr;
    ID3D11Texture2D* gShadowDepthTexture = nullptr;
    ID3D11DepthStencilView* gShadowDepthStencilView = nullptr;
    unsigned long gShadowWidth = 0;
    unsigned long gShadowHeight = 0;

    ID3D11VertexShader* gShadowVertexShader = nullptr;
    ID3D11PixelShader* gShadowPixelShaderModulateAlpha = nullptr;
    ID3D11PixelShader* gShadowPixelShaderDiffuseAlpha = nullptr;
    ID3D11InputLayout* gShadowInputLayout = nullptr;
    ID3D11RasterizerState* gShadowRasterizer = nullptr;
    ID3D11Texture2D* gShadowWhiteTexture = nullptr;
    ID3D11ShaderResourceView* gShadowWhiteSrv = nullptr;
    ID3D11Buffer* gShadowVertexBuffer = nullptr;
    size_t gShadowVertexBufferCapacity = 0;
    ID3D11Texture2D* gShadowSampleReadback = nullptr;

    std::unordered_map<unsigned long long, ID3D11DepthStencilState*> gShadowDepthStates;
    std::unordered_map<unsigned long long, ID3D11BlendState*> gShadowBlendStates;
    std::unordered_map<unsigned long long, ID3D11SamplerState*> gShadowSamplerStates;

    std::vector<ShadowGpuVertex> gShadowVertices;
    std::vector<ShadowCommand> gShadowCommands;

    unsigned long gShadowClearFlags = 3;
    unsigned long gShadowClearColor = 0xFF000000UL;
    float gShadowClearDepth = 1.0f;
    unsigned long gShadowClearStencil = 0;
    unsigned long gShadowSubmittedDraws = 0;
    unsigned long gShadowSkippedDraws = 0;
    int gShadowContinuous = 0;

    void Log(const char* format, ...)
    {
        FILE* file = std::fopen("spidey-decomp.log", "a");
        if (!file)
            return;

        std::fputs("[RENDERER11] ", file);

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

    template <typename T>
    void ReleaseStateMap(std::unordered_map<unsigned long long, T*>& states)
    {
        for (auto& entry : states)
            SafeRelease(entry.second);
        states.clear();
    }

    void ReleaseShadowTargets()
    {
        SafeRelease(gShadowColorSrv);
        SafeRelease(gShadowRenderTargetView);
        SafeRelease(gShadowColorTexture);
        SafeRelease(gShadowDepthStencilView);
        SafeRelease(gShadowDepthTexture);
        gShadowWidth = 0;
        gShadowHeight = 0;
    }

    void ReleaseShadowPipeline()
    {
        ReleaseShadowTargets();
        SafeRelease(gShadowSampleReadback);
        SafeRelease(gShadowVertexBuffer);
        gShadowVertexBufferCapacity = 0;
        SafeRelease(gShadowWhiteSrv);
        SafeRelease(gShadowWhiteTexture);
        SafeRelease(gShadowRasterizer);
        SafeRelease(gShadowInputLayout);
        SafeRelease(gShadowPixelShaderDiffuseAlpha);
        SafeRelease(gShadowPixelShaderModulateAlpha);
        SafeRelease(gShadowVertexShader);
        ReleaseStateMap(gShadowDepthStates);
        ReleaseStateMap(gShadowBlendStates);
        ReleaseStateMap(gShadowSamplerStates);
        gShadowVertices.clear();
        gShadowCommands.clear();
        gShadowSubmittedDraws = 0;
        gShadowSkippedDraws = 0;
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
        ReleaseShadowPipeline();
        ReleaseAllGameTexturesInternal();
        if (gSwapChain)
            gSwapChain->SetFullscreenState(FALSE, nullptr);
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


    bool ConvertShadowBlend(
        unsigned long legacyBlend,
        D3D11_BLEND& blend)
    {
        switch (legacyBlend)
        {
            case 1: blend = D3D11_BLEND_ZERO; return true;
            case 2: blend = D3D11_BLEND_ONE; return true;
            case 3: blend = D3D11_BLEND_SRC_COLOR; return true;
            case 4: blend = D3D11_BLEND_INV_SRC_COLOR; return true;
            case 5: blend = D3D11_BLEND_SRC_ALPHA; return true;
            case 6: blend = D3D11_BLEND_INV_SRC_ALPHA; return true;
            case 7: blend = D3D11_BLEND_DEST_ALPHA; return true;
            case 8: blend = D3D11_BLEND_INV_DEST_ALPHA; return true;
            case 9: blend = D3D11_BLEND_DEST_COLOR; return true;
            case 10: blend = D3D11_BLEND_INV_DEST_COLOR; return true;
            case 11: blend = D3D11_BLEND_SRC_ALPHA_SAT; return true;
            default: return false;
        }
    }

    bool ConvertShadowBlendAlpha(
        unsigned long legacyBlend,
        D3D11_BLEND& blend)
    {
        switch (legacyBlend)
        {
            case 1: blend = D3D11_BLEND_ZERO; return true;
            case 2: blend = D3D11_BLEND_ONE; return true;
            case 3: blend = D3D11_BLEND_SRC_ALPHA; return true;
            case 4: blend = D3D11_BLEND_INV_SRC_ALPHA; return true;
            case 5: blend = D3D11_BLEND_SRC_ALPHA; return true;
            case 6: blend = D3D11_BLEND_INV_SRC_ALPHA; return true;
            case 7: blend = D3D11_BLEND_DEST_ALPHA; return true;
            case 8: blend = D3D11_BLEND_INV_DEST_ALPHA; return true;
            case 9: blend = D3D11_BLEND_DEST_ALPHA; return true;
            case 10: blend = D3D11_BLEND_INV_DEST_ALPHA; return true;
            case 11: blend = D3D11_BLEND_ONE; return true;
            default: return false;
        }
    }

    ID3D11DepthStencilState* GetShadowDepthState(
        const SpideyRenderer11ShadowState& state)
    {
        if (!gDevice ||
            state.zFunc < 1 ||
            state.zFunc > 8)
        {
            return nullptr;
        }

        const unsigned long long key =
            (static_cast<unsigned long long>(state.zEnable ? 1UL : 0UL)) |
            (static_cast<unsigned long long>(state.zWrite ? 1UL : 0UL) << 8) |
            (static_cast<unsigned long long>(state.zFunc) << 16);

        const auto found = gShadowDepthStates.find(key);
        if (found != gShadowDepthStates.end())
            return found->second;

        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = state.zEnable ? TRUE : FALSE;
        desc.DepthWriteMask =
            state.zWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(state.zFunc);
        desc.StencilEnable = FALSE;

        ID3D11DepthStencilState* result = nullptr;
        const HRESULT hr =
            gDevice->CreateDepthStencilState(&desc, &result);

        if (FAILED(hr) || !result)
        {
            Log(
                "shadow depth_state failed hr=0x%08lX z=%lu zw=%lu zf=%lu",
                static_cast<unsigned long>(hr),
                state.zEnable,
                state.zWrite,
                state.zFunc);
            SafeRelease(result);
            return nullptr;
        }

        gShadowDepthStates[key] = result;
        return result;
    }

    ID3D11BlendState* GetShadowBlendState(
        const SpideyRenderer11ShadowState& state)
    {
        if (!gDevice)
            return nullptr;

        const unsigned long long key =
            (static_cast<unsigned long long>(state.alphaBlendEnable ? 1UL : 0UL)) |
            (static_cast<unsigned long long>(state.srcBlend) << 8) |
            (static_cast<unsigned long long>(state.dstBlend) << 16);

        const auto found = gShadowBlendStates.find(key);
        if (found != gShadowBlendStates.end())
            return found->second;

        D3D11_BLEND src = D3D11_BLEND_ONE;
        D3D11_BLEND dst = D3D11_BLEND_ZERO;

        if (state.alphaBlendEnable)
        {
            if (!ConvertShadowBlend(state.srcBlend, src) ||
                !ConvertShadowBlend(state.dstBlend, dst))
            {
                return nullptr;
            }
        }

        D3D11_BLEND_DESC desc = {};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable =
            state.alphaBlendEnable ? TRUE : FALSE;
        D3D11_BLEND srcAlpha = D3D11_BLEND_ONE;
        D3D11_BLEND dstAlpha = D3D11_BLEND_ZERO;

        if (state.alphaBlendEnable)
        {
            if (!ConvertShadowBlendAlpha(state.srcBlend, srcAlpha) ||
                !ConvertShadowBlendAlpha(state.dstBlend, dstAlpha))
            {
                return nullptr;
            }
        }

        desc.RenderTarget[0].SrcBlend = src;
        desc.RenderTarget[0].DestBlend = dst;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = srcAlpha;
        desc.RenderTarget[0].DestBlendAlpha = dstAlpha;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask =
            D3D11_COLOR_WRITE_ENABLE_ALL;

        ID3D11BlendState* result = nullptr;
        const HRESULT hr =
            gDevice->CreateBlendState(&desc, &result);

        if (FAILED(hr) || !result)
        {
            Log(
                "shadow blend_state failed hr=0x%08lX enable=%lu src=%lu dst=%lu",
                static_cast<unsigned long>(hr),
                state.alphaBlendEnable,
                state.srcBlend,
                state.dstBlend);
            SafeRelease(result);
            return nullptr;
        }

        gShadowBlendStates[key] = result;
        return result;
    }

    ID3D11SamplerState* GetShadowSamplerState(
        const SpideyRenderer11ShadowState& state)
    {
        if (!gDevice ||
            state.addressU < 1 ||
            state.addressU > 4 ||
            state.addressV < 1 ||
            state.addressV > 4)
        {
            return nullptr;
        }

        const bool linear =
            state.magFilter == 2 ||
            state.minFilter == 2;

        const unsigned long long key =
            (static_cast<unsigned long long>(state.addressU)) |
            (static_cast<unsigned long long>(state.addressV) << 8) |
            (static_cast<unsigned long long>(linear ? 1UL : 0UL) << 16);

        const auto found = gShadowSamplerStates.find(key);
        if (found != gShadowSamplerStates.end())
            return found->second;

        D3D11_SAMPLER_DESC desc = {};
        desc.Filter =
            linear ?
            D3D11_FILTER_MIN_MAG_MIP_LINEAR :
            D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU =
            static_cast<D3D11_TEXTURE_ADDRESS_MODE>(state.addressU);
        desc.AddressV =
            static_cast<D3D11_TEXTURE_ADDRESS_MODE>(state.addressV);
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MinLOD = 0.0f;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        ID3D11SamplerState* result = nullptr;
        const HRESULT hr =
            gDevice->CreateSamplerState(&desc, &result);

        if (FAILED(hr) || !result)
        {
            Log(
                "shadow sampler_state failed hr=0x%08lX u=%lu v=%lu linear=%d",
                static_cast<unsigned long>(hr),
                state.addressU,
                state.addressV,
                linear ? 1 : 0);
            SafeRelease(result);
            return nullptr;
        }

        gShadowSamplerStates[key] = result;
        return result;
    }

    bool CreateShadowPipeline()
    {
        if (!gDevice)
            return false;

        if (gShadowVertexShader &&
            gShadowPixelShaderModulateAlpha &&
            gShadowPixelShaderDiffuseAlpha &&
            gShadowInputLayout &&
            gShadowRasterizer &&
            gShadowWhiteSrv)
        {
            return true;
        }

        static const char* kShaderSource =
            "Texture2D gameTexture : register(t0);\n"
            "SamplerState gameSampler : register(s0);\n"
            "struct VSIn { float4 position : POSITION; float4 color : COLOR0; float2 uv : TEXCOORD0; };\n"
            "struct VSOut { float4 position : SV_POSITION; float4 color : COLOR0; float2 uvOverW : TEXCOORD0; float rhw : TEXCOORD1; };\n"
            "VSOut VSMain(VSIn input) {\n"
            "    VSOut output;\n"
            "    float rhw = abs(input.position.w) > 0.0000001 ? input.position.w : 1.0;\n"
            "    output.position = float4(input.position.xyz, 1.0);\n"
            "    output.color = input.color.bgra;\n"
            "    output.uvOverW = input.uv * rhw;\n"
            "    output.rhw = rhw;\n"
            "    return output;\n"
            "}\n"
            "float2 ResolveUv(VSOut input) {\n"
            "    float rhw = abs(input.rhw) > 0.0000001 ? input.rhw : 1.0;\n"
            "    return input.uvOverW / rhw;\n"
            "}\n"
            "float4 PSModulateAlpha(VSOut input) : SV_TARGET {\n"
            "    return gameTexture.Sample(gameSampler, ResolveUv(input)) * input.color;\n"
            "}\n"
            "float4 PSDiffuseAlpha(VSOut input) : SV_TARGET {\n"
            "    float4 texel = gameTexture.Sample(gameSampler, ResolveUv(input));\n"
            "    return float4(texel.rgb * input.color.rgb, input.color.a);\n"
            "}\n";

        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* psModBlob = nullptr;
        ID3DBlob* psDiffuseBlob = nullptr;
        ID3DBlob* errors = nullptr;

        HRESULT hr = D3DCompile(
            kShaderSource,
            std::strlen(kShaderSource),
            "spidey_shadow",
            nullptr,
            nullptr,
            "VSMain",
            "vs_4_0",
            D3DCOMPILE_ENABLE_STRICTNESS,
            0,
            &vsBlob,
            &errors);

        if (FAILED(hr))
        {
            Log(
                "shadow compile_vs failed hr=0x%08lX error=%s",
                static_cast<unsigned long>(hr),
                errors ? static_cast<const char*>(errors->GetBufferPointer()) : "none");
            SafeRelease(errors);
            SafeRelease(vsBlob);
            return false;
        }
        SafeRelease(errors);

        hr = D3DCompile(
            kShaderSource,
            std::strlen(kShaderSource),
            "spidey_shadow",
            nullptr,
            nullptr,
            "PSModulateAlpha",
            "ps_4_0",
            D3DCOMPILE_ENABLE_STRICTNESS,
            0,
            &psModBlob,
            &errors);

        if (FAILED(hr))
        {
            Log(
                "shadow compile_ps_modulate failed hr=0x%08lX error=%s",
                static_cast<unsigned long>(hr),
                errors ? static_cast<const char*>(errors->GetBufferPointer()) : "none");
            SafeRelease(errors);
            SafeRelease(psModBlob);
            SafeRelease(vsBlob);
            return false;
        }
        SafeRelease(errors);

        hr = D3DCompile(
            kShaderSource,
            std::strlen(kShaderSource),
            "spidey_shadow",
            nullptr,
            nullptr,
            "PSDiffuseAlpha",
            "ps_4_0",
            D3DCOMPILE_ENABLE_STRICTNESS,
            0,
            &psDiffuseBlob,
            &errors);

        if (FAILED(hr))
        {
            Log(
                "shadow compile_ps_diffuse failed hr=0x%08lX error=%s",
                static_cast<unsigned long>(hr),
                errors ? static_cast<const char*>(errors->GetBufferPointer()) : "none");
            SafeRelease(errors);
            SafeRelease(psDiffuseBlob);
            SafeRelease(psModBlob);
            SafeRelease(vsBlob);
            return false;
        }
        SafeRelease(errors);

        hr = gDevice->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr,
            &gShadowVertexShader);

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreatePixelShader(
                psModBlob->GetBufferPointer(),
                psModBlob->GetBufferSize(),
                nullptr,
                &gShadowPixelShaderModulateAlpha);
        }

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreatePixelShader(
                psDiffuseBlob->GetBufferPointer(),
                psDiffuseBlob->GetBufferSize(),
                nullptr,
                &gShadowPixelShaderDiffuseAlpha);
        }

        D3D11_INPUT_ELEMENT_DESC elements[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreateInputLayout(
                elements,
                static_cast<UINT>(sizeof(elements) / sizeof(elements[0])),
                vsBlob->GetBufferPointer(),
                vsBlob->GetBufferSize(),
                &gShadowInputLayout);
        }

        SafeRelease(psDiffuseBlob);
        SafeRelease(psModBlob);
        SafeRelease(vsBlob);

        if (FAILED(hr) ||
            !gShadowVertexShader ||
            !gShadowPixelShaderModulateAlpha ||
            !gShadowPixelShaderDiffuseAlpha ||
            !gShadowInputLayout)
        {
            Log("shadow create_shader_pipeline failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseShadowPipeline();
            return false;
        }

        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.DepthClipEnable = TRUE;

        hr = gDevice->CreateRasterizerState(
            &rasterizerDesc,
            &gShadowRasterizer);

        if (FAILED(hr) || !gShadowRasterizer)
        {
            Log("shadow create_rasterizer failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseShadowPipeline();
            return false;
        }

        const unsigned long whitePixel = 0xFFFFFFFFUL;
        D3D11_TEXTURE2D_DESC whiteDesc = {};
        whiteDesc.Width = 1;
        whiteDesc.Height = 1;
        whiteDesc.MipLevels = 1;
        whiteDesc.ArraySize = 1;
        whiteDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        whiteDesc.SampleDesc.Count = 1;
        whiteDesc.Usage = D3D11_USAGE_IMMUTABLE;
        whiteDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA whiteData = {};
        whiteData.pSysMem = &whitePixel;
        whiteData.SysMemPitch = sizeof(whitePixel);

        hr = gDevice->CreateTexture2D(
            &whiteDesc,
            &whiteData,
            &gShadowWhiteTexture);

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreateShaderResourceView(
                gShadowWhiteTexture,
                nullptr,
                &gShadowWhiteSrv);
        }

        if (FAILED(hr) || !gShadowWhiteTexture || !gShadowWhiteSrv)
        {
            Log("shadow create_white_texture failed hr=0x%08lX", static_cast<unsigned long>(hr));
            ReleaseShadowPipeline();
            return false;
        }

        gShadowVertices.reserve(65536);
        gShadowCommands.reserve(16384);

        Log("shadow pipeline ready shader_model=4_0 tl_vertex=screen_space manual_uv_perspective=1");
        return true;
    }

    bool EnsureShadowTargets(
        unsigned long width,
        unsigned long height)
    {
        if (!gDevice || width == 0 || height == 0)
            return false;

        if (gShadowColorTexture &&
            gShadowRenderTargetView &&
            gShadowDepthTexture &&
            gShadowDepthStencilView &&
            gShadowWidth == width &&
            gShadowHeight == height)
        {
            return true;
        }

        ReleaseShadowTargets();

        D3D11_TEXTURE2D_DESC colorDesc = {};
        colorDesc.Width = width;
        colorDesc.Height = height;
        colorDesc.MipLevels = 1;
        colorDesc.ArraySize = 1;
        colorDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        colorDesc.SampleDesc.Count = 1;
        colorDesc.Usage = D3D11_USAGE_DEFAULT;
        colorDesc.BindFlags =
            D3D11_BIND_RENDER_TARGET |
            D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = gDevice->CreateTexture2D(
            &colorDesc,
            nullptr,
            &gShadowColorTexture);

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreateRenderTargetView(
                gShadowColorTexture,
                nullptr,
                &gShadowRenderTargetView);
        }

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreateShaderResourceView(
                gShadowColorTexture,
                nullptr,
                &gShadowColorSrv);
        }

        if (FAILED(hr) ||
            !gShadowColorTexture ||
            !gShadowRenderTargetView ||
            !gShadowColorSrv)
        {
            Log(
                "shadow color_target failed hr=0x%08lX size=%lux%lu",
                static_cast<unsigned long>(hr),
                width,
                height);
            ReleaseShadowTargets();
            return false;
        }

        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        hr = gDevice->CreateTexture2D(
            &depthDesc,
            nullptr,
            &gShadowDepthTexture);

        if (SUCCEEDED(hr))
        {
            hr = gDevice->CreateDepthStencilView(
                gShadowDepthTexture,
                nullptr,
                &gShadowDepthStencilView);
        }

        if (FAILED(hr) ||
            !gShadowDepthTexture ||
            !gShadowDepthStencilView)
        {
            Log(
                "shadow depth_target failed hr=0x%08lX size=%lux%lu",
                static_cast<unsigned long>(hr),
                width,
                height);
            ReleaseShadowTargets();
            return false;
        }

        gShadowWidth = width;
        gShadowHeight = height;

        Log("shadow targets ready width=%lu height=%lu", width, height);
        return true;
    }

    bool DiagnosticShadowReadbackEnabled()
    {
        static int enabled = -1;
        if (enabled >= 0)
            return enabled != 0;

        char value[16] = {};
        const DWORD length =
            GetEnvironmentVariableA(
                "SPIDEY_RENDERER11_DIAG_READBACK",
                value,
                static_cast<DWORD>(sizeof(value)));

        enabled =
            length > 0 &&
            value[0] != '0' ?
            1 :
            0;

        Log(
            "diagnostic_shadow_readback enabled=%d source=%s",
            enabled,
            enabled ? "environment" : "default_off");
        return enabled != 0;
    }

    bool SampleShadowTarget(
        unsigned long& sampleHash,
        unsigned long& nonBlack,
        unsigned long samplePixels[9])
    {
        sampleHash = 2166136261UL;
        nonBlack = 0;
        for (unsigned long i = 0; i < 9; ++i)
            samplePixels[i] = 0;

        if (!gDevice ||
            !gContext ||
            !gShadowColorTexture ||
            gShadowWidth == 0 ||
            gShadowHeight == 0)
        {
            return false;
        }

        if (!gShadowSampleReadback)
        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = 3;
            desc.Height = 3;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_STAGING;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

            const HRESULT hr =
                gDevice->CreateTexture2D(
                    &desc,
                    nullptr,
                    &gShadowSampleReadback);

            if (FAILED(hr) || !gShadowSampleReadback)
            {
                Log(
                    "shadow sample_readback create_failed hr=0x%08lX",
                    static_cast<unsigned long>(hr));
                SafeRelease(gShadowSampleReadback);
                return false;
            }
        }

        const unsigned long xs[3] =
        {
            gShadowWidth / 4UL,
            gShadowWidth / 2UL,
            (gShadowWidth * 3UL) / 4UL
        };

        const unsigned long ys[3] =
        {
            gShadowHeight / 4UL,
            gShadowHeight / 2UL,
            (gShadowHeight * 3UL) / 4UL
        };

        for (unsigned long y = 0; y < 3; ++y)
        {
            for (unsigned long x = 0; x < 3; ++x)
            {
                D3D11_BOX box = {};
                box.left = xs[x];
                box.top = ys[y];
                box.front = 0;
                box.right = xs[x] + 1;
                box.bottom = ys[y] + 1;
                box.back = 1;

                gContext->CopySubresourceRegion(
                    gShadowSampleReadback,
                    0,
                    x,
                    y,
                    0,
                    gShadowColorTexture,
                    0,
                    &box);
            }
        }

        D3D11_MAPPED_SUBRESOURCE mapped = {};
        const HRESULT hr =
            gContext->Map(
                gShadowSampleReadback,
                0,
                D3D11_MAP_READ,
                0,
                &mapped);

        if (FAILED(hr) || !mapped.pData)
        {
            Log(
                "shadow sample_readback map_failed hr=0x%08lX",
                static_cast<unsigned long>(hr));
            return false;
        }

        for (unsigned long y = 0; y < 3; ++y)
        {
            const unsigned char* row =
                static_cast<const unsigned char*>(mapped.pData) +
                static_cast<size_t>(y) * mapped.RowPitch;

            for (unsigned long x = 0; x < 3; ++x)
            {
                const unsigned char* pixel =
                    row + static_cast<size_t>(x) * 4U;

                const unsigned long colorRef =
                    static_cast<unsigned long>(pixel[2]) |
                    (static_cast<unsigned long>(pixel[1]) << 8) |
                    (static_cast<unsigned long>(pixel[0]) << 16);

                samplePixels[y * 3 + x] = colorRef;
                sampleHash ^= colorRef;
                sampleHash *= 16777619UL;

                if (colorRef != 0)
                    ++nonBlack;
            }
        }

        gContext->Unmap(
            gShadowSampleReadback,
            0);
        return true;
    }

    bool EnsureShadowVertexBuffer(size_t requiredBytes)
    {
        if (!gDevice || requiredBytes == 0)
            return false;

        if (gShadowVertexBuffer &&
            gShadowVertexBufferCapacity >= requiredBytes)
        {
            return true;
        }

        SafeRelease(gShadowVertexBuffer);
        gShadowVertexBufferCapacity = 0;

        size_t capacity = 4096;
        while (capacity < requiredBytes)
            capacity *= 2;

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = static_cast<UINT>(capacity);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        const HRESULT hr =
            gDevice->CreateBuffer(
                &desc,
                nullptr,
                &gShadowVertexBuffer);

        if (FAILED(hr) || !gShadowVertexBuffer)
        {
            Log(
                "shadow vertex_buffer failed hr=0x%08lX bytes=%llu",
                static_cast<unsigned long>(hr),
                static_cast<unsigned long long>(capacity));
            SafeRelease(gShadowVertexBuffer);
            return false;
        }

        gShadowVertexBufferCapacity = capacity;

        Log(
            "shadow vertex_buffer ready bytes=%llu",
            static_cast<unsigned long long>(capacity));
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
int __cdecl SpideyRenderer11_SetFullscreenState(
    int exclusive,
    unsigned long width,
    unsigned long height)
{
    if (!gSwapChain)
        return 0;

    HRESULT hr = S_OK;

    if (exclusive)
    {
        DXGI_MODE_DESC mode = {};
        mode.Width = width ? width : gWidth;
        mode.Height = height ? height : gHeight;
        mode.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        mode.RefreshRate.Numerator = 0;
        mode.RefreshRate.Denominator = 0;
        mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        // Enter exclusive ownership first, then request the selected display
        // mode. Calling ResizeTarget while still windowed only resizes the
        // window and does not establish a true exclusive display mode.
        hr = gSwapChain->SetFullscreenState(TRUE, nullptr);
        if (SUCCEEDED(hr))
            hr = gSwapChain->ResizeTarget(&mode);

        if (FAILED(hr))
            gSwapChain->SetFullscreenState(FALSE, nullptr);
    }
    else
    {
        hr = gSwapChain->SetFullscreenState(FALSE, nullptr);
    }

    Log(
        "fullscreen_state exclusive=%d width=%lu height=%lu hr=0x%08lX",
        exclusive ? 1 : 0,
        width,
        height,
        static_cast<unsigned long>(hr));

    return SUCCEEDED(hr) ? 1 : 0;
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
long __cdecl SpideyRenderer11_UpdateTransientTexture(
    unsigned long legacyHandle,
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
    if (!legacyHandle)
        return -1;

    const long existing =
        SpideyRenderer11_ResolveTextureHandle(legacyHandle);

    if (existing >= 0)
        return existing;

    unsigned long slot = kGameTextureCapacity;

    for (unsigned long textureId = kPersistentTextureCapacity;
         textureId < kGameTextureCapacity;
         ++textureId)
    {
        if (!gGameTextures[textureId].texture &&
            !gGameTextures[textureId].srv &&
            !gGameTextures[textureId].legacyHandle)
        {
            slot = textureId;
            break;
        }
    }

    if (slot >= kGameTextureCapacity)
    {
        Log("transient_texture no_free_slot handle=0x%08lX", legacyHandle);
        return -1;
    }

    if (!SpideyRenderer11_UpdateTexture(
            slot,
            pixels,
            width,
            height,
            pitch,
            bitsPerPixel,
            redMask,
            greenMask,
            blueMask,
            alphaMask))
    {
        return -1;
    }

    if (!SpideyRenderer11_AssociateTextureHandle(
            slot,
            legacyHandle))
    {
        SpideyRenderer11_ReleaseTexture(slot);
        return -1;
    }

    Log(
        "transient_texture id=%lu handle=0x%08lX size=%lux%lu bpp=%lu",
        slot,
        legacyHandle,
        width,
        height,
        bitsPerPixel);

    return static_cast<long>(slot);
}

extern "C" __declspec(dllexport)
void __cdecl SpideyRenderer11_ShadowSetClear(
    unsigned long clearFlags,
    unsigned long clearColor,
    float clearDepth,
    unsigned long clearStencil)
{
    gShadowClearFlags = clearFlags;
    gShadowClearColor = clearColor;
    gShadowClearDepth = clearDepth;
    gShadowClearStencil = clearStencil;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_ShadowSubmitTriangleFan(
    const SpideyRenderer11ShadowVertex* vertices,
    unsigned long vertexCount,
    const SpideyRenderer11ShadowState* state)
{
    if (!vertices ||
        !state ||
        vertexCount < 3 ||
        state->viewportWidth == 0 ||
        state->viewportHeight == 0)
    {
        ++gShadowSkippedDraws;
        return 0;
    }

    // Phase 2C1 intentionally supports exactly the fixed-function subset
    // observed in the live retail stream. Unsupported states remain on D3D7
    // and are counted rather than being approximated silently.
    if (state->fogEnable ||
        state->alphaTestEnable ||
        state->colorOp != 4 ||
        state->colorArg1 != 2 ||
        state->colorArg2 != 0 ||
        (state->alphaOp != 3 && state->alphaOp != 4) ||
        (state->alphaOp == 4 &&
         (state->alphaArg1 != 2 || state->alphaArg2 != 0)))
    {
        ++gShadowSkippedDraws;
        return 0;
    }

    long textureId = -1;
    if (state->textureHandle)
    {
        textureId =
            SpideyRenderer11_ResolveTextureHandle(
                state->textureHandle);

        // Keep unresolved legacy handles in the command. The proxy mirrors
        // transient DirectDraw surfaces after EndScene and before this frame
        // is replayed, so ShadowEndFrame gets a second chance to resolve it.
        if (textureId < 0)
            textureId = -2;
    }

    ShadowCommand command = {};
    command.firstVertex =
        static_cast<unsigned long>(gShadowVertices.size());
    command.vertexCount =
        (vertexCount - 2UL) * 3UL;
    command.textureId = textureId;
    command.state = *state;

    const float viewportX =
        static_cast<float>(state->viewportX);
    const float viewportY =
        static_cast<float>(state->viewportY);
    const float viewportWidth =
        static_cast<float>(state->viewportWidth);
    const float viewportHeight =
        static_cast<float>(state->viewportHeight);

    auto appendVertex =
        [&](const SpideyRenderer11ShadowVertex& source)
        {
            const float ndcX =
                ((source.x - viewportX) / viewportWidth) * 2.0f - 1.0f;
            const float ndcY =
                1.0f - ((source.y - viewportY) / viewportHeight) * 2.0f;

            // D3D7 XYZRHW vertices are already transformed into screen
            // space. Keep clip W fixed so DX11 clips them linearly in the
            // same screen-space domain instead of reconstructing a varying
            // homogeneous W (which warps very large off-screen triangles).
            // Carry RHW in the otherwise-unused input position.w; the shader
            // uses it only to reconstruct fixed-function perspective texture
            // interpolation after clipping.
            ShadowGpuVertex output = {};
            output.x = ndcX;
            output.y = ndcY;
            output.z = source.z;
            output.w = source.rhw;
            output.diffuse = source.diffuse;
            output.u = source.u;
            output.v = source.v;
            gShadowVertices.push_back(output);
        };

    for (unsigned long i = 1;
         i + 1 < vertexCount;
         ++i)
    {
        appendVertex(vertices[0]);
        appendVertex(vertices[i]);
        appendVertex(vertices[i + 1]);
    }

    gShadowCommands.push_back(command);
    ++gShadowSubmittedDraws;
    return 1;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_ShadowEndFrame(
    unsigned long frame,
    unsigned long sceneWidth,
    unsigned long sceneHeight)
{
    const unsigned long submitted =
        gShadowSubmittedDraws;
    const unsigned long skippedSubmit =
        gShadowSkippedDraws;
    const size_t queuedVertices =
        gShadowVertices.size();
    const size_t queuedCommands =
        gShadowCommands.size();

    auto resetFrame =
        []()
        {
            gShadowVertices.clear();
            gShadowCommands.clear();
            gShadowSubmittedDraws = 0;
            gShadowSkippedDraws = 0;
        };

    if (queuedCommands == 0)
    {
        if (frame <= 5 || (frame % 120) == 0 || skippedSubmit)
        {
            Log(
                "shadow_frame frame=%lu target=%lux%lu queued=0 submitted=%lu skipped_submit=%lu rendered=0 skipped_render=0 vertices=0 presentable=0",
                frame,
                sceneWidth,
                sceneHeight,
                submitted,
                skippedSubmit);
        }

        resetFrame();
        // An empty command stream is not a presentable DX11 frame. Returning
        // success here previously let the proxy seize exclusive ownership
        // before any shadow render target/SRV existed.
        return 0;
    }

    const bool replayThisFrame =
        gShadowContinuous ||
        frame <= 5 ||
        (frame % 120) == 0;

    if (!replayThisFrame)
    {
        if (skippedSubmit)
        {
            Log(
                "shadow_frame frame=%lu target=%lux%lu replay=0 queued=%llu submitted=%lu skipped_submit=%lu rendered=0 skipped_render=0 vertices=%llu",
                frame,
                sceneWidth,
                sceneHeight,
                static_cast<unsigned long long>(queuedCommands),
                submitted,
                skippedSubmit,
                static_cast<unsigned long long>(queuedVertices));
        }

        resetFrame();
        // No replay means no newly completed presentable frame.
        return 0;
    }

    const size_t requiredBytes =
        queuedVertices * sizeof(ShadowGpuVertex);

    if (!CreateShadowPipeline() ||
        !EnsureShadowTargets(sceneWidth, sceneHeight) ||
        !EnsureShadowVertexBuffer(requiredBytes))
    {
        Log(
            "shadow_frame frame=%lu setup_failed target=%lux%lu queued=%llu vertices=%llu",
            frame,
            sceneWidth,
            sceneHeight,
            static_cast<unsigned long long>(queuedCommands),
            static_cast<unsigned long long>(queuedVertices));
        resetFrame();
        return 0;
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = gContext->Map(
        gShadowVertexBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped);

    if (FAILED(hr) || !mapped.pData)
    {
        Log(
            "shadow_frame frame=%lu map_failed hr=0x%08lX bytes=%llu",
            frame,
            static_cast<unsigned long>(hr),
            static_cast<unsigned long long>(requiredBytes));
        resetFrame();
        return 0;
    }

    std::memcpy(
        mapped.pData,
        gShadowVertices.data(),
        requiredBytes);
    gContext->Unmap(gShadowVertexBuffer, 0);

    gContext->OMSetRenderTargets(
        1,
        &gShadowRenderTargetView,
        gShadowDepthStencilView);

    if (gShadowClearFlags & 1UL)
    {
        const float clearColor[4] =
        {
            static_cast<float>((gShadowClearColor >> 16) & 0xFFUL) / 255.0f,
            static_cast<float>((gShadowClearColor >> 8) & 0xFFUL) / 255.0f,
            static_cast<float>(gShadowClearColor & 0xFFUL) / 255.0f,
            static_cast<float>((gShadowClearColor >> 24) & 0xFFUL) / 255.0f
        };

        gContext->ClearRenderTargetView(
            gShadowRenderTargetView,
            clearColor);
    }

    UINT clearDepthFlags = 0;
    if (gShadowClearFlags & 2UL)
        clearDepthFlags |= D3D11_CLEAR_DEPTH;
    if (gShadowClearFlags & 4UL)
        clearDepthFlags |= D3D11_CLEAR_STENCIL;

    if (clearDepthFlags)
    {
        gContext->ClearDepthStencilView(
            gShadowDepthStencilView,
            clearDepthFlags,
            gShadowClearDepth,
            static_cast<UINT8>(gShadowClearStencil & 0xFFUL));
    }

    const UINT stride =
        static_cast<UINT>(sizeof(ShadowGpuVertex));
    const UINT offset = 0;

    gContext->IASetVertexBuffers(
        0,
        1,
        &gShadowVertexBuffer,
        &stride,
        &offset);
    gContext->IASetInputLayout(gShadowInputLayout);
    gContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gContext->VSSetShader(
        gShadowVertexShader,
        nullptr,
        0);
    gContext->RSSetState(
        gShadowRasterizer);

    unsigned long rendered = 0;
    unsigned long skippedRender = 0;

    for (const ShadowCommand& command : gShadowCommands)
    {
        ID3D11DepthStencilState* depthState =
            GetShadowDepthState(command.state);
        ID3D11BlendState* blendState =
            GetShadowBlendState(command.state);
        ID3D11SamplerState* samplerState =
            GetShadowSamplerState(command.state);

        if (!depthState ||
            !blendState ||
            !samplerState)
        {
            ++skippedRender;
            continue;
        }

        ID3D11ShaderResourceView* srv =
            gShadowWhiteSrv;

        long resolvedTextureId =
            command.textureId;

        if (command.state.textureHandle)
        {
            resolvedTextureId =
                SpideyRenderer11_ResolveTextureHandle(
                    command.state.textureHandle);

            if (resolvedTextureId < 0)
            {
                ++skippedRender;
                continue;
            }
        }

        if (resolvedTextureId >= 0 &&
            static_cast<unsigned long>(resolvedTextureId) < kGameTextureCapacity &&
            gGameTextures[resolvedTextureId].srv)
        {
            srv =
                gGameTextures[resolvedTextureId].srv;
        }

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX =
            static_cast<float>(command.state.viewportX);
        viewport.TopLeftY =
            static_cast<float>(command.state.viewportY);
        viewport.Width =
            static_cast<float>(command.state.viewportWidth);
        viewport.Height =
            static_cast<float>(command.state.viewportHeight);
        viewport.MinDepth =
            command.state.viewportMinZ;
        viewport.MaxDepth =
            command.state.viewportMaxZ;

        gContext->RSSetViewports(
            1,
            &viewport);
        gContext->OMSetDepthStencilState(
            depthState,
            0);
        gContext->OMSetBlendState(
            blendState,
            nullptr,
            0xFFFFFFFFUL);
        gContext->PSSetShader(
            command.state.alphaOp == 4 ?
            gShadowPixelShaderModulateAlpha :
            gShadowPixelShaderDiffuseAlpha,
            nullptr,
            0);
        gContext->PSSetShaderResources(
            0,
            1,
            &srv);
        gContext->PSSetSamplers(
            0,
            1,
            &samplerState);

        gContext->Draw(
            command.vertexCount,
            command.firstVertex);
        ++rendered;
    }

    ID3D11ShaderResourceView* nullSrv = nullptr;
    gContext->PSSetShaderResources(
        0,
        1,
        &nullSrv);
    gContext->OMSetRenderTargets(
        0,
        nullptr,
        nullptr);
    gContext->OMSetBlendState(
        nullptr,
        nullptr,
        0xFFFFFFFFUL);
    gContext->OMSetDepthStencilState(
        nullptr,
        0);
    gContext->RSSetState(nullptr);

    unsigned long sampleHash = 0;
    unsigned long sampleNonBlack = 0;
    unsigned long samplePixels[9] = {};
    // CPU readback of the render target is a blocking GPU synchronization
    // point. It was useful while validating DX11 replay, but it causes a
    // visible periodic hitch when sampled every 120 frames. Keep the
    // diagnostic available only as an explicit opt-in.
    const int shouldSamplePixels =
        DiagnosticShadowReadbackEnabled() &&
        (frame <= 5 ||
         (frame % 120) == 0);
    const int sampled =
        shouldSamplePixels &&
        SampleShadowTarget(
            sampleHash,
            sampleNonBlack,
            samplePixels) ? 1 : 0;

    if (frame <= 5 ||
        (frame % 120) == 0 ||
        skippedSubmit ||
        skippedRender ||
        rendered != queuedCommands)
    {
        Log(
            "shadow_frame frame=%lu target=%lux%lu replay=1 queued=%llu submitted=%lu skipped_submit=%lu rendered=%lu skipped_render=%lu vertices=%llu sampled=%d sample_hash=0x%08lX nonblack=%lu samples=%06lX,%06lX,%06lX,%06lX,%06lX,%06lX,%06lX,%06lX,%06lX presentable=%d",
            frame,
            sceneWidth,
            sceneHeight,
            static_cast<unsigned long long>(queuedCommands),
            submitted,
            skippedSubmit,
            rendered,
            skippedRender,
            static_cast<unsigned long long>(queuedVertices),
            sampled,
            sampleHash,
            sampleNonBlack,
            samplePixels[0],
            samplePixels[1],
            samplePixels[2],
            samplePixels[3],
            samplePixels[4],
            samplePixels[5],
            samplePixels[6],
            samplePixels[7],
            samplePixels[8],
            (rendered > 0 &&
             rendered == queuedCommands &&
             skippedSubmit == 0 &&
             skippedRender == 0) ? 1 : 0);
    }

    const int presentable =
        rendered > 0 &&
        rendered == queuedCommands &&
        skippedSubmit == 0 &&
        skippedRender == 0 ? 1 : 0;

    resetFrame();
    return presentable;
}

extern "C" __declspec(dllexport)
void __cdecl SpideyRenderer11_ShadowSetContinuous(
    int enabled)
{
    gShadowContinuous = enabled ? 1 : 0;
    Log("shadow_continuous enabled=%d", gShadowContinuous);
}

extern "C" __declspec(dllexport)
int __cdecl SpideyRenderer11_PresentShadow(
    int preserveAspect,
    int vsync)
{
    if (!gSwapChain ||
        !gContext ||
        !gWindow ||
        !gShadowColorSrv ||
        !gShadowWidth ||
        !gShadowHeight)
    {
        Log(
            "present_shadow rejected swap=0x%p context=0x%p hwnd=0x%p srv=0x%p shadow=%lux%lu",
            gSwapChain,
            gContext,
            gWindow,
            gShadowColorSrv,
            gShadowWidth,
            gShadowHeight);
        return 0;
    }

    RECT client = {};
    if (!GetClientRect(gWindow, &client))
        return 0;

    const unsigned long targetWidth =
        static_cast<unsigned long>(client.right - client.left);
    const unsigned long targetHeight =
        static_cast<unsigned long>(client.bottom - client.top);

    if (!targetWidth || !targetHeight)
        return 0;

    if (targetWidth != gWidth || targetHeight != gHeight)
    {
        if (!SpideyRenderer11_Resize(targetWidth, targetHeight))
            return 0;
    }

    if (!CreateBlitPipeline())
        return 0;

    unsigned long presentWidth = targetWidth;
    unsigned long presentHeight = targetHeight;
    unsigned long presentX = 0;
    unsigned long presentY = 0;

    if (preserveAspect)
    {
        const unsigned long long srcWide =
            static_cast<unsigned long long>(gShadowWidth) * targetHeight;
        const unsigned long long dstWide =
            static_cast<unsigned long long>(targetWidth) * gShadowHeight;

        if (srcWide > dstWide)
        {
            presentHeight =
                static_cast<unsigned long>(
                    static_cast<unsigned long long>(targetWidth) *
                    gShadowHeight / gShadowWidth);
            presentY = (targetHeight - presentHeight) / 2;
        }
        else if (srcWide < dstWide)
        {
            presentWidth =
                static_cast<unsigned long>(
                    static_cast<unsigned long long>(targetHeight) *
                    gShadowWidth / gShadowHeight);
            presentX = (targetWidth - presentWidth) / 2;
        }
    }

    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    gContext->OMSetRenderTargets(
        1,
        &gRenderTargetView,
        nullptr);
    gContext->ClearRenderTargetView(
        gRenderTargetView,
        clearColor);

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
    gContext->PSSetShaderResources(0, 1, &gShadowColorSrv);
    gContext->PSSetSamplers(0, 1, &gBlitSampler);
    gContext->Draw(3, 0);

    ID3D11ShaderResourceView* nullSrv = nullptr;
    gContext->PSSetShaderResources(0, 1, &nullSrv);

    const HRESULT hr =
        gSwapChain->Present(vsync ? 1 : 0, 0);

    if (FAILED(hr))
    {
        Log(
            "present_shadow present_failed hr=0x%08lX",
            static_cast<unsigned long>(hr));
        return 0;
    }

    static unsigned long shadowPresentFrame = 0;
    ++shadowPresentFrame;

    if (shadowPresentFrame <= 5 ||
        (shadowPresentFrame % 120) == 0)
    {
        Log(
            "present_shadow frame=%lu src=%lux%lu dst=%lux%lu rect=%lu,%lu,%lux%lu aspect=%d vsync=%d",
            shadowPresentFrame,
            gShadowWidth,
            gShadowHeight,
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
