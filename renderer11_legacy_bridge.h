#pragma once

#ifndef SPIDEY_RENDERER11_LEGACY_BRIDGE_H
#define SPIDEY_RENDERER11_LEGACY_BRIDGE_H

int SpideyRenderer11MirrorLegacyTexture(
    unsigned long textureId,
    void* legacySurface);

int SpideyRenderer11AssociateLegacyTexture(
    unsigned long textureId,
    void* legacySurface);

void SpideyRenderer11ReleaseMirroredTexture(
    unsigned long textureId);

void SpideyRenderer11ReleaseAllMirroredTextures(void);

unsigned long SpideyRenderer11GetMirroredTextureCount(void);

#endif
