#include "spidey_input11_api.h"

#include <cstdio>
#include <cstring>

int main()
{
    const unsigned long abi =
        SpideyInput11_GetAbiVersion();
    const char* backend =
        SpideyInput11_GetBackendName();
    const int probe =
        SpideyInput11_Probe();

    std::printf(
        "abi=%lu expected=%lu backend=%s probe=%d\n",
        abi,
        SPIDEY_INPUT11_ABI_VERSION,
        backend ? backend : "unknown",
        probe);

    if (abi != SPIDEY_INPUT11_ABI_VERSION ||
        !probe)
    {
        SpideyInput11_Shutdown();
        return 2;
    }

    SpideyInput11State state;
    std::memset(
        &state,
        0,
        sizeof(state));

    const int poll =
        SpideyInput11_Poll(
            &state,
            sizeof(state));

    std::printf(
        "poll=%d connected=%lu family=%lu user=%lu packet=%lu buttons=0x%08lX move=%.4f,%.4f camera=%.4f,%.4f triggers=%.4f,%.4f sequence=%lu\n",
        poll,
        state.connected,
        state.deviceFamily,
        state.userIndex,
        state.packetNumber,
        state.buttons,
        state.moveX,
        state.moveY,
        state.cameraX,
        state.cameraY,
        state.leftTrigger,
        state.rightTrigger,
        state.sequence);

    SpideyInput11_Shutdown();

    return poll ? 0 : 3;
}
