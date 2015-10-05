#pragma once
#ifndef __EMSCRIPTEN__
#include <Core/Common.hpp>
#include <SDL2/SDL_mixer.h>

#define AUDIO_STEREO 2
#define AUDIO_MONO 1

#define AUDIO_DISABLED 1
#define AUDIO_FREQUENCY MIX_DEFAULT_FREQUENCY * 2
#define AUDIO_FORMAT MIX_DEFAULT_FORMAT
#define AUDIO_CHANNELS MIX_DEFAULT_CHANNELS
#define AUDIO_CHUNK_SIZE 1024

struct AudioSettings {
    bool soundEnabled = false;
    bool musicEnabled = false;
    F32 soundVolume  = 1.0f;
    F32 musicVolume  = 1.0f;
    F32 masterVolume = 1.0f;
};

namespace Audio {

void Init();
void Terminate();

}

Mix_Chunk* LoadSound(const char* filename);
void PlaySound(Mix_Chunk* sound, U32 loopCount = 0);

Mix_Music* LoadMusic(const char* filename);
void PlayMusic(Mix_Music* music, U32 loops = -1);

#endif