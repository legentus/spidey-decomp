#include "spidey_input11_api.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <Xinput.h>

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace
{
    using XInputGetStateFn = DWORD (WINAPI *)(DWORD, XINPUT_STATE*);
    using XInputSetStateFn = DWORD (WINAPI *)(DWORD, XINPUT_VIBRATION*);

    HMODULE gXInputModule = nullptr;
    XInputGetStateFn gXInputGetState = nullptr;
    XInputSetStateFn gXInputSetState = nullptr;

    unsigned long gSequence = 0;
    DWORD gActiveUser = XUSER_MAX_COUNT;
    bool gWasConnected = false;
    bool gLoadAttempted = false;

    void Log(const char* format, ...)
    {
        FILE* file = std::fopen("spidey-input11.log", "a");
        if (!file)
            return;

        va_list args;
        va_start(args, format);
        std::vfprintf(file, format, args);
        va_end(args);

        std::fputc('\n', file);
        std::fclose(file);
    }

    bool LoadXInput()
    {
        if (gXInputModule &&
            gXInputGetState &&
            gXInputSetState)
        {
            return true;
        }

        if (gLoadAttempted)
            return false;

        gLoadAttempted = true;

        static const char* kCandidates[] =
        {
            "xinput1_4.dll",
            "xinput1_3.dll",
            "xinput9_1_0.dll",
            "xinput1_2.dll",
            "xinput1_1.dll"
        };

        for (const char* candidate : kCandidates)
        {
            HMODULE module = LoadLibraryA(candidate);
            if (!module)
                continue;

            auto getState =
                reinterpret_cast<XInputGetStateFn>(
                    GetProcAddress(module, "XInputGetState"));
            auto setState =
                reinterpret_cast<XInputSetStateFn>(
                    GetProcAddress(module, "XInputSetState"));

            if (!getState ||
                !setState)
            {
                FreeLibrary(module);
                continue;
            }

            gXInputModule = module;
            gXInputGetState = getState;
            gXInputSetState = setState;

            Log(
                "backend loaded provider=xinput dll=%s abi=%lu",
                candidate,
                SPIDEY_INPUT11_ABI_VERSION);
            return true;
        }

        Log(
            "backend unavailable provider=xinput error=%lu",
            static_cast<unsigned long>(GetLastError()));
        return false;
    }

    float NormalizeTrigger(BYTE value)
    {
        constexpr float kThreshold =
            static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

        if (static_cast<float>(value) <= kThreshold)
            return 0.0f;

        const float normalized =
            (static_cast<float>(value) - kThreshold) /
            (255.0f - kThreshold);

        return std::clamp(normalized, 0.0f, 1.0f);
    }

    void NormalizeStick(
        SHORT rawX,
        SHORT rawY,
        SHORT deadzone,
        float& outX,
        float& outY)
    {
        const float x =
            static_cast<float>(rawX);
        const float y =
            static_cast<float>(rawY);

        const float magnitude =
            std::sqrt(x * x + y * y);

        if (magnitude <=
            static_cast<float>(deadzone))
        {
            outX = 0.0f;
            outY = 0.0f;
            return;
        }

        const float clampedMagnitude =
            std::min(magnitude, 32767.0f);
        const float legalRange =
            32767.0f -
            static_cast<float>(deadzone);

        const float scaledMagnitude =
            std::clamp(
                (clampedMagnitude -
                 static_cast<float>(deadzone)) /
                    legalRange,
                0.0f,
                1.0f);

        if (magnitude <= 0.0f)
        {
            outX = 0.0f;
            outY = 0.0f;
            return;
        }

        outX =
            (x / magnitude) *
            scaledMagnitude;
        outY =
            (y / magnitude) *
            scaledMagnitude;
    }

    unsigned long MapButtons(WORD buttons)
    {
        unsigned long result = 0;

        if (buttons & XINPUT_GAMEPAD_A)
            result |= SPIDEY_INPUT_BUTTON_A;
        if (buttons & XINPUT_GAMEPAD_B)
            result |= SPIDEY_INPUT_BUTTON_B;
        if (buttons & XINPUT_GAMEPAD_X)
            result |= SPIDEY_INPUT_BUTTON_X;
        if (buttons & XINPUT_GAMEPAD_Y)
            result |= SPIDEY_INPUT_BUTTON_Y;
        if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER)
            result |= SPIDEY_INPUT_BUTTON_LB;
        if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
            result |= SPIDEY_INPUT_BUTTON_RB;
        if (buttons & XINPUT_GAMEPAD_BACK)
            result |= SPIDEY_INPUT_BUTTON_BACK;
        if (buttons & XINPUT_GAMEPAD_START)
            result |= SPIDEY_INPUT_BUTTON_START;
        if (buttons & XINPUT_GAMEPAD_LEFT_THUMB)
            result |= SPIDEY_INPUT_BUTTON_LS;
        if (buttons & XINPUT_GAMEPAD_RIGHT_THUMB)
            result |= SPIDEY_INPUT_BUTTON_RS;
        if (buttons & XINPUT_GAMEPAD_DPAD_UP)
            result |= SPIDEY_INPUT_BUTTON_DPAD_UP;
        if (buttons & XINPUT_GAMEPAD_DPAD_DOWN)
            result |= SPIDEY_INPUT_BUTTON_DPAD_DOWN;
        if (buttons & XINPUT_GAMEPAD_DPAD_LEFT)
            result |= SPIDEY_INPUT_BUTTON_DPAD_LEFT;
        if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT)
            result |= SPIDEY_INPUT_BUTTON_DPAD_RIGHT;

        return result;
    }

    bool ReadFirstConnected(
        DWORD& userIndex,
        XINPUT_STATE& state)
    {
        if (!gXInputGetState)
            return false;

        if (gActiveUser < XUSER_MAX_COUNT)
        {
            std::memset(
                &state,
                0,
                sizeof(state));

            if (gXInputGetState(
                    gActiveUser,
                    &state) == ERROR_SUCCESS)
            {
                userIndex = gActiveUser;
                return true;
            }
        }

        for (DWORD index = 0;
             index < XUSER_MAX_COUNT;
             ++index)
        {
            std::memset(
                &state,
                0,
                sizeof(state));

            if (gXInputGetState(
                    index,
                    &state) == ERROR_SUCCESS)
            {
                userIndex = index;
                return true;
            }
        }

        return false;
    }
}

extern "C" __declspec(dllexport)
unsigned long __cdecl SpideyInput11_GetAbiVersion(void)
{
    return SPIDEY_INPUT11_ABI_VERSION;
}

extern "C" __declspec(dllexport)
const char* __cdecl SpideyInput11_GetBackendName(void)
{
    return "spidey_input11/xinput-dynamic";
}

extern "C" __declspec(dllexport)
int __cdecl SpideyInput11_Probe(void)
{
    return LoadXInput() ? 1 : 0;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyInput11_Poll(
    SpideyInput11State* state,
    unsigned long stateSize)
{
    if (!state ||
        stateSize < sizeof(SpideyInput11State))
    {
        return 0;
    }

    std::memset(
        state,
        0,
        sizeof(*state));

    state->structSize =
        sizeof(*state);
    state->sequence =
        ++gSequence;
    state->deviceFamily =
        SPIDEY_INPUT_DEVICE_NONE;
    state->userIndex =
        0xFFFFFFFFUL;

    if (!LoadXInput())
        return 0;

    DWORD userIndex =
        XUSER_MAX_COUNT;
    XINPUT_STATE xinputState;
    std::memset(
        &xinputState,
        0,
        sizeof(xinputState));

    if (!ReadFirstConnected(
            userIndex,
            xinputState))
    {
        if (gWasConnected)
        {
            Log(
                "controller disconnected previous_user=%lu sequence=%lu",
                static_cast<unsigned long>(gActiveUser),
                state->sequence);
        }

        gWasConnected = false;
        gActiveUser = XUSER_MAX_COUNT;
        return 1;
    }

    const bool connectionChanged =
        !gWasConnected ||
        gActiveUser != userIndex;

    gWasConnected = true;
    gActiveUser = userIndex;

    state->connected = 1;
    state->deviceFamily =
        SPIDEY_INPUT_DEVICE_XINPUT;
    state->userIndex =
        static_cast<unsigned long>(userIndex);
    state->packetNumber =
        static_cast<unsigned long>(
            xinputState.dwPacketNumber);
    state->buttons =
        MapButtons(
            xinputState.Gamepad.wButtons);

    NormalizeStick(
        xinputState.Gamepad.sThumbLX,
        xinputState.Gamepad.sThumbLY,
        XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
        state->moveX,
        state->moveY);

    NormalizeStick(
        xinputState.Gamepad.sThumbRX,
        xinputState.Gamepad.sThumbRY,
        XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
        state->cameraX,
        state->cameraY);

    state->leftTrigger =
        NormalizeTrigger(
            xinputState.Gamepad.bLeftTrigger);
    state->rightTrigger =
        NormalizeTrigger(
            xinputState.Gamepad.bRightTrigger);

    if (connectionChanged)
    {
        Log(
            "controller connected provider=xinput user=%lu sequence=%lu",
            state->userIndex,
            state->sequence);
    }

    return 1;
}

extern "C" __declspec(dllexport)
int __cdecl SpideyInput11_SetVibration(
    float lowFrequency,
    float highFrequency)
{
    if (!LoadXInput() ||
        !gXInputSetState ||
        gActiveUser >= XUSER_MAX_COUNT)
    {
        return 0;
    }

    const float low =
        std::clamp(
            lowFrequency,
            0.0f,
            1.0f);
    const float high =
        std::clamp(
            highFrequency,
            0.0f,
            1.0f);

    XINPUT_VIBRATION vibration = {};
    vibration.wLeftMotorSpeed =
        static_cast<WORD>(
            low * 65535.0f);
    vibration.wRightMotorSpeed =
        static_cast<WORD>(
            high * 65535.0f);

    return gXInputSetState(
               gActiveUser,
               &vibration) ==
           ERROR_SUCCESS ?
        1 :
        0;
}

extern "C" __declspec(dllexport)
void __cdecl SpideyInput11_Shutdown(void)
{
    if (gXInputSetState &&
        gActiveUser < XUSER_MAX_COUNT)
    {
        XINPUT_VIBRATION vibration = {};
        gXInputSetState(
            gActiveUser,
            &vibration);
    }

    if (gXInputModule)
    {
        FreeLibrary(
            gXInputModule);
    }

    gXInputModule = nullptr;
    gXInputGetState = nullptr;
    gXInputSetState = nullptr;
    gActiveUser = XUSER_MAX_COUNT;
    gWasConnected = false;
    gLoadAttempted = false;

    Log("backend shutdown");
}
