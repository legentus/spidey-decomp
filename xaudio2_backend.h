#pragma once

#ifndef SPIDEY_XAUDIO2_BACKEND_H
#define SPIDEY_XAUDIO2_BACKEND_H

#ifdef _WIN32

int SpideyXAudio2Initialize(void);
void SpideyXAudio2Shutdown(void);
int SpideyXAudio2IsActive(void);

void __cdecl SpideyXAudio2Open(int voiceIndex, int soundIndex, int levelBank);
void __cdecl SpideyXAudio2Close(int voiceIndex);
void __cdecl SpideyXAudio2Play(int voiceIndex, int loop);
void __cdecl SpideyXAudio2Stop(int voiceIndex);
void __cdecl SpideyXAudio2SetVolume(int voiceIndex, int volume);
void __cdecl SpideyXAudio2SetPan(int voiceIndex, int pan);
void __cdecl SpideyXAudio2SetPitch(int voiceIndex, int pitch);
int __cdecl SpideyXAudio2IsPlaying(int voiceIndex);

#endif

#endif
