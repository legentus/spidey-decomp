#pragma once

#ifndef SPIDEY_XAUDIO2_COMPAT_H
#define SPIDEY_XAUDIO2_COMPAT_H

#ifdef _WIN32

#include <windows.h>
#include <mmsystem.h>

// Minimal XAudio2 2.9 ABI declarations kept VC6-compatible.  We dynamically
// load xaudio2_9.dll, so the matching build does not need modern SDK headers
// or xaudio2.lib.

struct SpideyIXAudio2;
struct SpideyIXAudio2SourceVoice;
struct SpideyIXAudio2MasteringVoice;

struct SpideyXAudio2Buffer
{
    UINT32 Flags;
    UINT32 AudioBytes;
    const BYTE* pAudioData;
    UINT32 PlayBegin;
    UINT32 PlayLength;
    UINT32 LoopBegin;
    UINT32 LoopLength;
    UINT32 LoopCount;
    void* pContext;
};

struct SpideyXAudio2VoiceState
{
    void* pCurrentBufferContext;
    UINT32 BuffersQueued;
    unsigned __int64 SamplesPlayed;
};

struct SpideyXAudio2VoiceDetails
{
    UINT32 CreationFlags;
    UINT32 ActiveFlags;
    UINT32 InputChannels;
    UINT32 InputSampleRate;
};

struct SpideyIXAudio2Vtbl
{
    HRESULT (WINAPI *QueryInterface)(SpideyIXAudio2*, REFIID, void**);
    ULONG (WINAPI *AddRef)(SpideyIXAudio2*);
    ULONG (WINAPI *Release)(SpideyIXAudio2*);
    HRESULT (WINAPI *RegisterForCallbacks)(SpideyIXAudio2*, void*);
    void (WINAPI *UnregisterForCallbacks)(SpideyIXAudio2*, void*);
    HRESULT (WINAPI *CreateSourceVoice)(
        SpideyIXAudio2*,
        SpideyIXAudio2SourceVoice**,
        const WAVEFORMATEX*,
        UINT32,
        float,
        void*,
        const void*,
        const void*);
    HRESULT (WINAPI *CreateSubmixVoice)(
        SpideyIXAudio2*,
        void**,
        UINT32,
        UINT32,
        UINT32,
        UINT32,
        const void*,
        const void*);
    HRESULT (WINAPI *CreateMasteringVoice)(
        SpideyIXAudio2*,
        SpideyIXAudio2MasteringVoice**,
        UINT32,
        UINT32,
        UINT32,
        LPCWSTR,
        const void*,
        int);
    HRESULT (WINAPI *StartEngine)(SpideyIXAudio2*);
    void (WINAPI *StopEngine)(SpideyIXAudio2*);
    HRESULT (WINAPI *CommitChanges)(SpideyIXAudio2*, UINT32);
    void (WINAPI *GetPerformanceData)(SpideyIXAudio2*, void*);
    void (WINAPI *SetDebugConfiguration)(SpideyIXAudio2*, const void*, void*);
};

struct SpideyIXAudio2
{
    SpideyIXAudio2Vtbl* lpVtbl;
};

struct SpideyIXAudio2VoiceVtbl
{
    void (WINAPI *GetVoiceDetails)(void*, SpideyXAudio2VoiceDetails*);
    HRESULT (WINAPI *SetOutputVoices)(void*, const void*);
    HRESULT (WINAPI *SetEffectChain)(void*, const void*);
    HRESULT (WINAPI *EnableEffect)(void*, UINT32, UINT32);
    HRESULT (WINAPI *DisableEffect)(void*, UINT32, UINT32);
    void (WINAPI *GetEffectState)(void*, UINT32, BOOL*);
    HRESULT (WINAPI *SetEffectParameters)(void*, UINT32, const void*, UINT32, UINT32);
    HRESULT (WINAPI *GetEffectParameters)(void*, UINT32, void*, UINT32);
    HRESULT (WINAPI *SetFilterParameters)(void*, const void*, UINT32);
    void (WINAPI *GetFilterParameters)(void*, void*);
    HRESULT (WINAPI *SetOutputFilterParameters)(void*, void*, const void*, UINT32);
    void (WINAPI *GetOutputFilterParameters)(void*, void*, void*);
    HRESULT (WINAPI *SetVolume)(void*, float, UINT32);
    void (WINAPI *GetVolume)(void*, float*);
    HRESULT (WINAPI *SetChannelVolumes)(void*, UINT32, const float*, UINT32);
    void (WINAPI *GetChannelVolumes)(void*, UINT32, float*);
    HRESULT (WINAPI *SetOutputMatrix)(void*, void*, UINT32, UINT32, const float*, UINT32);
    void (WINAPI *GetOutputMatrix)(void*, void*, UINT32, UINT32, float*);
    void (WINAPI *DestroyVoice)(void*);
    HRESULT (WINAPI *Start)(void*, UINT32, UINT32);
    HRESULT (WINAPI *Stop)(void*, UINT32, UINT32);
    HRESULT (WINAPI *SubmitSourceBuffer)(void*, const SpideyXAudio2Buffer*, const void*);
    HRESULT (WINAPI *FlushSourceBuffers)(void*);
    HRESULT (WINAPI *Discontinuity)(void*);
    HRESULT (WINAPI *ExitLoop)(void*, UINT32);
    void (WINAPI *GetState)(void*, SpideyXAudio2VoiceState*, UINT32);
    HRESULT (WINAPI *SetFrequencyRatio)(void*, float, UINT32);
    void (WINAPI *GetFrequencyRatio)(void*, float*);
    HRESULT (WINAPI *SetSourceSampleRate)(void*, UINT32);
};

struct SpideyIXAudio2SourceVoice
{
    SpideyIXAudio2VoiceVtbl* lpVtbl;
};

struct SpideyIXAudio2MasteringVoice
{
    SpideyIXAudio2VoiceVtbl* lpVtbl;
};

typedef HRESULT (WINAPI *SpideyXAudio2CreateFn)(
    SpideyIXAudio2**,
    UINT32,
    UINT32);

#define SPIDEY_XAUDIO2_END_OF_STREAM 0x0040
#define SPIDEY_XAUDIO2_LOOP_INFINITE 255
#define SPIDEY_XAUDIO2_DEFAULT_PROCESSOR 0x00000001
#define SPIDEY_XAUDIO2_COMMIT_NOW 0
#define SPIDEY_XAUDIO2_MAX_RATIO 1024.0f

#endif

#endif
