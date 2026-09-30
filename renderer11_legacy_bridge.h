#pragma once

#ifndef SPIDEY_RENDERER11_LEGACY_BRIDGE_H
#define SPIDEY_RENDERER11_LEGACY_BRIDGE_H

struct SpideyRenderer11LegacyShadowVertex
{
    float x;
    float y;
    float z;
    float rhw;
    unsigned long diffuse;
    float u;
    float v;
};

struct SpideyRenderer11LegacyShadowState
{
    unsigned long textureHandle;
    unsigned long viewportX;
    unsigned long viewportY;
    unsigned long viewportWidth;
    unsigned long viewportHeight;
    float viewportMinZ;
    float viewportMaxZ;
    unsigned long zEnable;
    unsigned long zWrite;
    unsigned long zFunc;
    unsigned long alphaBlendEnable;
    unsigned long srcBlend;
    unsigned long dstBlend;
    unsigned long alphaTestEnable;
    unsigned long alphaRef;
    unsigned long alphaFunc;
    unsigned long fogEnable;
    unsigned long fogColor;
    unsigned long colorOp;
    unsigned long colorArg1;
    unsigned long colorArg2;
    unsigned long alphaOp;
    unsigned long alphaArg1;
    unsigned long alphaArg2;
    unsigned long addressU;
    unsigned long addressV;
    unsigned long magFilter;
    unsigned long minFilter;
};

int SpideyRenderer11MirrorLegacyTexture(
    unsigned long textureId,
    void* legacySurface);

int SpideyRenderer11AssociateLegacyTexture(
    unsigned long textureId,
    void* legacySurface);

long SpideyRenderer11ResolveLegacyTexture(
    void* legacySurface);

long SpideyRenderer11MirrorTransientLegacyTexture(
    void* legacySurface);

void SpideyRenderer11ShadowSetClear(
    unsigned long clearFlags,
    unsigned long clearColor,
    float clearDepth,
    unsigned long clearStencil);

int SpideyRenderer11ShadowSubmitTriangleFan(
    const SpideyRenderer11LegacyShadowVertex* vertices,
    unsigned long vertexCount,
    const SpideyRenderer11LegacyShadowState* state);

int SpideyRenderer11ShadowEndFrame(
    unsigned long frame,
    unsigned long sceneWidth,
    unsigned long sceneHeight);

void SpideyRenderer11ShadowSetContinuous(
    int enabled);

int SpideyRenderer11PresentShadow(
    int preserveAspect,
    int vsync);

void SpideyRenderer11ReleaseMirroredTexture(
    unsigned long textureId);

void SpideyRenderer11ReleaseAllMirroredTextures(void);

unsigned long SpideyRenderer11GetMirroredTextureCount(void);

#endif
