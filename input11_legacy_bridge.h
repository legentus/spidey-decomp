#pragma once

#ifndef SPIDEY_INPUT11_LEGACY_BRIDGE_H
#define SPIDEY_INPUT11_LEGACY_BRIDGE_H

struct SpideyInput11LegacyState
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
};

int SpideyInput11PassivePoll(
    unsigned long frame);

int SpideyInput11IsConnected(void);

const SpideyInput11LegacyState* SpideyInput11GetState(void);

#endif
