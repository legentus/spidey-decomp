#include "xaudio2_backend.h"

#ifdef _WIN32

#include "DXsound.h"
#include "xaudio2_compat.h"

#include <windows.h>
#include <objbase.h>
#include <dsound.h>
#include <mmsystem.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>

// The Visual C++ 6.0 platform headers predate CoInitializeEx even though the
// function is exported by ole32.dll on every Windows version we support.
// Keep a local ABI declaration so the matching compiler can call it.
extern "C" HRESULT WINAPI CoInitializeEx(
        LPVOID pvReserved,
        DWORD dwCoInit);
#ifndef COINIT_MULTITHREADED
#define COINIT_MULTITHREADED 0x0
#endif

struct SpideyXAudio2VoiceSlot
{
    SpideyIXAudio2SourceVoice* voice;
    unsigned char* audioData;
    unsigned long audioBytes;
    unsigned long formatBytes;
    unsigned char formatStorage[128];
    unsigned long inputChannels;
    unsigned long sampleRate;
    int loop;
    int opened;
};

static HMODULE gSpideyXAudio2Module = 0;
static SpideyIXAudio2* gSpideyXAudio2 = 0;
static SpideyIXAudio2MasteringVoice* gSpideyXAudio2Master = 0;
static unsigned long gSpideyXAudio2OutputChannels = 2;
static int gSpideyXAudio2Active = 0;
static int gSpideyXAudio2ComInitialized = 0;
static int gSpideyXAudio2LoggedL1A1Alias1 = 0;
static SpideyXAudio2VoiceSlot gSpideyXAudio2Voices[32];

static void SpideyXAudio2Log(
        const char* text)
{
    FILE* f = fopen(
        "spidey-audio.log",
        "a");
    if (!f)
        return;

    fprintf(
        f,
        "%s\n",
        text ? text : "");
    fclose(f);
}

static void SpideyXAudio2LogHr(
        const char* eventName,
        HRESULT hr)
{
    char line[192];
    sprintf(
        line,
        "%s hr=0x%08lX",
        eventName ? eventName : "xaudio2_event",
        (unsigned long)hr);
    SpideyXAudio2Log(
        line);
}

static void SpideyXAudio2ReleaseCom(void)
{
    if (gSpideyXAudio2ComInitialized)
    {
        CoUninitialize();
        gSpideyXAudio2ComInitialized = 0;
    }
}

static int SpideyXAudio2ForcedDirectSound()
{
    char value[64];
    DWORD n =
        GetEnvironmentVariableA(
            "SPIDEY_AUDIO_BACKEND",
            value,
            sizeof(value));
    if (!n ||
        n >= sizeof(value))
    {
        return 0;
    }

    return
        !_stricmp(value, "directsound") ||
        !_stricmp(value, "dsound") ||
        !_stricmp(value, "legacy");
}

static IDirectSoundBuffer* SpideyRetailSourceBuffer(
        int soundIndex,
        int levelBank)
{
    if (levelBank)
        soundIndex += 0x40;

    if (soundIndex < 0 ||
        soundIndex >= 0x80)
    {
        return 0;
    }

    IDirectSoundBuffer** retailBuffers =
        (IDirectSoundBuffer**)0x006BBAD4;

    return retailBuffers[soundIndex];
}

static void SpideyXAudio2DestroyVoice(
        int voiceIndex)
{
    if (voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];

    if (slot->voice)
    {
        slot->voice->lpVtbl->Stop(
            slot->voice,
            0,
            SPIDEY_XAUDIO2_COMMIT_NOW);
        slot->voice->lpVtbl->FlushSourceBuffers(
            slot->voice);
        slot->voice->lpVtbl->DestroyVoice(
            slot->voice);
        slot->voice = 0;
    }

    if (slot->audioData)
    {
        free(
            slot->audioData);
        slot->audioData = 0;
    }

    memset(
        slot,
        0,
        sizeof(*slot));
}

static float SpideyClampFloat(
        float value,
        float low,
        float high)
{
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

int SpideyXAudio2Initialize(void)
{
    if (gSpideyXAudio2Active)
        return 1;

    if (SpideyXAudio2ForcedDirectSound())
    {
        SpideyXAudio2Log(
            "backend=directsound reason=environment_override");
        return 0;
    }

    // XAudio2 2.9 requires COM to be initialized on the calling thread.
    // The retail game never needed to do this for DirectSound, so initialize
    // COM here before creating the XAudio2 mastering voice. If this thread is
    // already initialized in a different apartment model, COM is still usable
    // and we must not balance that case with CoUninitialize().
    HRESULT comHr =
        CoInitializeEx(
            0,
            COINIT_MULTITHREADED);
    if (SUCCEEDED(comHr))
    {
        gSpideyXAudio2ComInitialized = 1;
    }
    else if (comHr != RPC_E_CHANGED_MODE)
    {
        SpideyXAudio2LogHr(
            "backend=directsound reason=com_initialize_failed",
            comHr);
        return 0;
    }

    memset(
        gSpideyXAudio2Voices,
        0,
        sizeof(gSpideyXAudio2Voices));

    gSpideyXAudio2Module =
        LoadLibraryA(
            "xaudio2_9.dll");
    const char* moduleName =
        "xaudio2_9.dll";

    if (!gSpideyXAudio2Module)
    {
        gSpideyXAudio2Module =
            LoadLibraryA(
                "xaudio2_9redist.dll");
        moduleName =
            "xaudio2_9redist.dll";
    }

    if (!gSpideyXAudio2Module)
    {
        SpideyXAudio2Log(
            "backend=directsound reason=xaudio2_9_not_found");
        SpideyXAudio2ReleaseCom();
        return 0;
    }

    SpideyXAudio2CreateFn createFn =
        (SpideyXAudio2CreateFn)GetProcAddress(
            gSpideyXAudio2Module,
            "XAudio2Create");
    if (!createFn)
    {
        SpideyXAudio2Log(
            "backend=directsound reason=xaudio2_create_missing");
        FreeLibrary(
            gSpideyXAudio2Module);
        gSpideyXAudio2Module = 0;
        SpideyXAudio2ReleaseCom();
        return 0;
    }

    HRESULT hr =
        createFn(
            &gSpideyXAudio2,
            0,
            SPIDEY_XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr) ||
        !gSpideyXAudio2)
    {
        SpideyXAudio2LogHr(
            "backend=directsound reason=xaudio2_create_failed",
            hr);
        FreeLibrary(
            gSpideyXAudio2Module);
        gSpideyXAudio2Module = 0;
        gSpideyXAudio2 = 0;
        SpideyXAudio2ReleaseCom();
        return 0;
    }

    hr =
        gSpideyXAudio2->lpVtbl->CreateMasteringVoice(
            gSpideyXAudio2,
            &gSpideyXAudio2Master,
            0,
            0,
            0,
            0,
            0,
            6); // AudioCategory_GameEffects

    if (FAILED(hr) ||
        !gSpideyXAudio2Master)
    {
        SpideyXAudio2LogHr(
            "backend=directsound reason=mastering_voice_failed",
            hr);
        gSpideyXAudio2->lpVtbl->Release(
            gSpideyXAudio2);
        gSpideyXAudio2 = 0;
        FreeLibrary(
            gSpideyXAudio2Module);
        gSpideyXAudio2Module = 0;
        SpideyXAudio2ReleaseCom();
        return 0;
    }

    SpideyXAudio2VoiceDetails details;
    memset(
        &details,
        0,
        sizeof(details));
    gSpideyXAudio2Master->lpVtbl->GetVoiceDetails(
        gSpideyXAudio2Master,
        &details);
    if (details.InputChannels)
        gSpideyXAudio2OutputChannels =
            details.InputChannels;
    if (gSpideyXAudio2OutputChannels > 8)
        gSpideyXAudio2OutputChannels = 8;

    hr =
        gSpideyXAudio2->lpVtbl->StartEngine(
            gSpideyXAudio2);
    if (FAILED(hr))
    {
        gSpideyXAudio2Master->lpVtbl->DestroyVoice(
            gSpideyXAudio2Master);
        gSpideyXAudio2Master = 0;
        gSpideyXAudio2->lpVtbl->Release(
            gSpideyXAudio2);
        gSpideyXAudio2 = 0;
        FreeLibrary(
            gSpideyXAudio2Module);
        gSpideyXAudio2Module = 0;
        SpideyXAudio2LogHr(
            "backend=directsound reason=start_engine_failed",
            hr);
        SpideyXAudio2ReleaseCom();
        return 0;
    }

    gSpideyXAudio2Active =
        1;

    char line[160];
    sprintf(
        line,
        "backend=xaudio2 module=%s output_channels=%lu phase=voice_backend_v1",
        moduleName,
        gSpideyXAudio2OutputChannels);
    SpideyXAudio2Log(
        line);

    return 1;
}

void SpideyXAudio2Shutdown(void)
{
    if (!gSpideyXAudio2Active)
        return;

    for (int i = 0;
        i < 32;
        ++i)
    {
        SpideyXAudio2DestroyVoice(
            i);
    }

    if (gSpideyXAudio2Master)
    {
        gSpideyXAudio2Master->lpVtbl->DestroyVoice(
            gSpideyXAudio2Master);
        gSpideyXAudio2Master = 0;
    }

    if (gSpideyXAudio2)
    {
        gSpideyXAudio2->lpVtbl->StopEngine(
            gSpideyXAudio2);
        gSpideyXAudio2->lpVtbl->Release(
            gSpideyXAudio2);
        gSpideyXAudio2 = 0;
    }

    if (gSpideyXAudio2Module)
    {
        FreeLibrary(
            gSpideyXAudio2Module);
        gSpideyXAudio2Module = 0;
    }

    gSpideyXAudio2Active =
        0;
    SpideyXAudio2ReleaseCom();
}

int SpideyXAudio2IsActive(void)
{
    return gSpideyXAudio2Active;
}

void __cdecl SpideyXAudio2Open(
        int voiceIndex,
        int soundIndex,
        int levelBank)
{
    if (!gSpideyXAudio2Active ||
        voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return;
    }

    SpideyXAudio2DestroyVoice(
        voiceIndex);

    IDirectSoundBuffer* source =
        SpideyRetailSourceBuffer(
            soundIndex,
            levelBank);
    if (!source)
        return;

    DSBCAPS caps;
    memset(
        &caps,
        0,
        sizeof(caps));
    caps.dwSize =
        sizeof(caps);

    HRESULT hr =
        source->GetCaps(
            &caps);
    if (FAILED(hr) ||
        !caps.dwBufferBytes)
    {
        return;
    }

    DWORD formatBytes = 0;
    hr =
        source->GetFormat(
            0,
            0,
            &formatBytes);
    if (FAILED(hr) ||
        !formatBytes ||
        formatBytes > 128)
    {
        return;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];

    WAVEFORMATEX* format =
        (WAVEFORMATEX*)slot->formatStorage;
    memset(
        slot->formatStorage,
        0,
        sizeof(slot->formatStorage));

    hr =
        source->GetFormat(
            format,
            formatBytes,
            &formatBytes);
    if (FAILED(hr))
    {
        memset(
            slot,
            0,
            sizeof(*slot));
        return;
    }

    slot->audioData =
        (unsigned char*)malloc(
            caps.dwBufferBytes);
    if (!slot->audioData)
    {
        memset(
            slot,
            0,
            sizeof(*slot));
        return;
    }

    void* p1 = 0;
    void* p2 = 0;
    DWORD b1 = 0;
    DWORD b2 = 0;

    hr =
        source->Lock(
            0,
            caps.dwBufferBytes,
            &p1,
            &b1,
            &p2,
            &b2,
            0);
    if (FAILED(hr))
    {
        free(
            slot->audioData);
        memset(
            slot,
            0,
            sizeof(*slot));
        return;
    }

    if (p1 &&
        b1)
    {
        memcpy(
            slot->audioData,
            p1,
            b1);
    }

    if (p2 &&
        b2)
    {
        memcpy(
            slot->audioData + b1,
            p2,
            b2);
    }

    source->Unlock(
        p1,
        b1,
        p2,
        b2);

    slot->audioBytes =
        b1 + b2;
    slot->formatBytes =
        formatBytes;
    slot->inputChannels =
        format->nChannels;
    slot->sampleRate =
        format->nSamplesPerSec;

    if (levelBank &&
        soundIndex == 33 &&
        !gSpideyXAudio2LoggedL1A1Alias1)
    {
        char line[256];
        sprintf(
            line,
            "l1a1_alias1_xaudio_source asset=33 format_tag=%u channels=%u rate=%lu bits=%u block_align=%u bytes=%lu",
            (unsigned int)format->wFormatTag,
            (unsigned int)format->nChannels,
            (unsigned long)format->nSamplesPerSec,
            (unsigned int)format->wBitsPerSample,
            (unsigned int)format->nBlockAlign,
            (unsigned long)slot->audioBytes);
        SpideyXAudio2Log(
            line);
        gSpideyXAudio2LoggedL1A1Alias1 = 1;
    }

    hr =
        gSpideyXAudio2->lpVtbl->CreateSourceVoice(
            gSpideyXAudio2,
            &slot->voice,
            format,
            0,
            SPIDEY_XAUDIO2_MAX_RATIO,
            0,
            0,
            0);

    if (FAILED(hr) ||
        !slot->voice)
    {
        free(
            slot->audioData);
        memset(
            slot,
            0,
            sizeof(*slot));
        return;
    }

    slot->opened =
        1;
}

void __cdecl SpideyXAudio2Close(
        int voiceIndex)
{
    SpideyXAudio2DestroyVoice(
        voiceIndex);
}

void __cdecl SpideyXAudio2Play(
        int voiceIndex,
        int loop)
{
    if (!gSpideyXAudio2Active ||
        voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];
    if (!slot->voice ||
        !slot->audioData ||
        !slot->audioBytes)
    {
        return;
    }

    slot->voice->lpVtbl->Stop(
        slot->voice,
        0,
        SPIDEY_XAUDIO2_COMMIT_NOW);
    slot->voice->lpVtbl->FlushSourceBuffers(
        slot->voice);

    SpideyXAudio2Buffer buffer;
    memset(
        &buffer,
        0,
        sizeof(buffer));
    buffer.Flags =
        SPIDEY_XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes =
        slot->audioBytes;
    buffer.pAudioData =
        slot->audioData;
    buffer.LoopCount =
        loop ?
            SPIDEY_XAUDIO2_LOOP_INFINITE :
            0;

    HRESULT hr =
        slot->voice->lpVtbl->SubmitSourceBuffer(
            slot->voice,
            &buffer,
            0);
    if (FAILED(hr))
        return;

    slot->loop =
        loop != 0;

    slot->voice->lpVtbl->Start(
        slot->voice,
        0,
        SPIDEY_XAUDIO2_COMMIT_NOW);
}

void __cdecl SpideyXAudio2Stop(
        int voiceIndex)
{
    if (!gSpideyXAudio2Active ||
        voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];
    if (!slot->voice)
        return;

    slot->voice->lpVtbl->Stop(
        slot->voice,
        0,
        SPIDEY_XAUDIO2_COMMIT_NOW);
    slot->voice->lpVtbl->FlushSourceBuffers(
        slot->voice);
}

void __cdecl SpideyXAudio2SetVolume(
        int voiceIndex,
        int volume)
{
    if (!gSpideyXAudio2Active ||
        voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];
    if (!slot->voice)
        return;

    const int dsHundredthDb =
        39 * (volume - 255);
    float gain =
        (float)pow(
            10.0,
            (double)dsHundredthDb / 2000.0);
    gain =
        SpideyClampFloat(
            gain,
            0.0f,
            16.0f);

    slot->voice->lpVtbl->SetVolume(
        slot->voice,
        gain,
        SPIDEY_XAUDIO2_COMMIT_NOW);
}

void __cdecl SpideyXAudio2SetPan(
        int voiceIndex,
        int pan)
{
    if (!gSpideyXAudio2Active ||
        voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];
    if (!slot->voice ||
        !gSpideyXAudio2Master)
    {
        return;
    }

    if (slot->inputChannels == 0 ||
        slot->inputChannels > 2 ||
        gSpideyXAudio2OutputChannels == 0)
    {
        return;
    }

    float normalized =
        ((float)pan - 15.5f) /
        15.5f;
    normalized =
        SpideyClampFloat(
            normalized,
            -1.0f,
            1.0f);

    float left =
        normalized > 0.0f ?
            1.0f - normalized :
            1.0f;
    float right =
        normalized < 0.0f ?
            1.0f + normalized :
            1.0f;

    float matrix[16];
    memset(
        matrix,
        0,
        sizeof(matrix));

    const unsigned long dst =
        gSpideyXAudio2OutputChannels;

    if (slot->inputChannels == 1)
    {
        if (dst >= 1)
            matrix[0] = left;
        if (dst >= 2)
            matrix[1] = right;
    }
    else
    {
        if (dst >= 1)
            matrix[0] = left;
        if (dst >= 2)
            matrix[1] = right;

        if (dst >= 1)
            matrix[dst] = left;
        if (dst >= 2)
            matrix[dst + 1] = right;
    }

    slot->voice->lpVtbl->SetOutputMatrix(
        slot->voice,
        gSpideyXAudio2Master,
        slot->inputChannels,
        dst,
        matrix,
        SPIDEY_XAUDIO2_COMMIT_NOW);
}

void __cdecl SpideyXAudio2SetPitch(
        int voiceIndex,
        int pitch)
{
    if (!gSpideyXAudio2Active ||
        voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];
    if (!slot->voice)
        return;

    float ratio =
        (float)pitch /
        1200.0f;
    ratio =
        SpideyClampFloat(
            ratio,
            1.0f / 1024.0f,
            SPIDEY_XAUDIO2_MAX_RATIO);

    slot->voice->lpVtbl->SetFrequencyRatio(
        slot->voice,
        ratio,
        SPIDEY_XAUDIO2_COMMIT_NOW);
}

int __cdecl SpideyXAudio2IsPlaying(
        int voiceIndex)
{
    if (!gSpideyXAudio2Active ||
        voiceIndex < 0 ||
        voiceIndex >= 32)
    {
        return 0;
    }

    SpideyXAudio2VoiceSlot* slot =
        &gSpideyXAudio2Voices[voiceIndex];
    if (!slot->voice)
        return 0;

    SpideyXAudio2VoiceState state;
    memset(
        &state,
        0,
        sizeof(state));

    slot->voice->lpVtbl->GetState(
        slot->voice,
        &state,
        0);

    return
        state.BuffersQueued != 0;
}

#endif
