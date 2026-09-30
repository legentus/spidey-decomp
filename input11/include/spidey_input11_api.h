#pragma once

#define SPIDEY_INPUT11_ABI_VERSION 1UL

#define SPIDEY_INPUT_DEVICE_NONE   0UL
#define SPIDEY_INPUT_DEVICE_XINPUT 1UL

#define SPIDEY_INPUT_BUTTON_A          0x00000001UL
#define SPIDEY_INPUT_BUTTON_B          0x00000002UL
#define SPIDEY_INPUT_BUTTON_X          0x00000004UL
#define SPIDEY_INPUT_BUTTON_Y          0x00000008UL
#define SPIDEY_INPUT_BUTTON_LB         0x00000010UL
#define SPIDEY_INPUT_BUTTON_RB         0x00000020UL
#define SPIDEY_INPUT_BUTTON_BACK       0x00000040UL
#define SPIDEY_INPUT_BUTTON_START      0x00000080UL
#define SPIDEY_INPUT_BUTTON_LS         0x00000100UL
#define SPIDEY_INPUT_BUTTON_RS         0x00000200UL
#define SPIDEY_INPUT_BUTTON_DPAD_UP    0x00000400UL
#define SPIDEY_INPUT_BUTTON_DPAD_DOWN  0x00000800UL
#define SPIDEY_INPUT_BUTTON_DPAD_LEFT  0x00001000UL
#define SPIDEY_INPUT_BUTTON_DPAD_RIGHT 0x00002000UL

typedef struct SpideyInput11State
{
    unsigned long structSize;
    unsigned long sequence;
    unsigned long connected;
    unsigned long deviceFamily;
    unsigned long userIndex;
    unsigned long packetNumber;
    unsigned long buttons;
    float moveX;
    float moveY;
    float cameraX;
    float cameraY;
    float leftTrigger;
    float rightTrigger;
} SpideyInput11State;

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) unsigned long __cdecl SpideyInput11_GetAbiVersion(void);
__declspec(dllexport) const char* __cdecl SpideyInput11_GetBackendName(void);
__declspec(dllexport) int __cdecl SpideyInput11_Probe(void);
__declspec(dllexport) int __cdecl SpideyInput11_Poll(
    SpideyInput11State* state,
    unsigned long stateSize);
__declspec(dllexport) int __cdecl SpideyInput11_SetVibration(
    float lowFrequency,
    float highFrequency);
__declspec(dllexport) void __cdecl SpideyInput11_Shutdown(void);

#ifdef __cplusplus
}
#endif
