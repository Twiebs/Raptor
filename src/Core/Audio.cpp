#include "Audio.hpp"
#ifndef __EMSCRIPTEN__

#include <Core/logging.h>

void Audio::Init () {
	int mixFlags = MIX_INIT_OGG | MIX_INIT_FLAC | MIX_INIT_MP3;
	int mixInitialized = Mix_Init(mixFlags);
	if (mixInitialized & mixFlags != mixFlags) {
		LOG_ERROR("AUDIO: Failed to init required audio library support");
		LOG_ERROR("AUDIO: " << Mix_GetError());
		return;
	}

	if (Mix_OpenAudio(AUDIO_FREQUENCY, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNK_SIZE)) {
		LOG_ERROR("AUDIO: Failed to initalize audio context" << Mix_GetError());
		return;
	}
}

Mix_Chunk* LoadSound(const char* filename) {
    auto chunk = Mix_LoadWAV(filename);
    if (chunk == nullptr) {
        LOG_ERROR("Failed to load sound: " << filename << ": " << Mix_GetError());
        return nullptr;
    }
    return chunk;
}

void PlaySound(Mix_Chunk* sound, U32 loopCount) {
    if (AUDIO_DISABLED) return;
    auto channel = Mix_PlayChannel(-1, sound, loopCount);
    if (channel == -1) {
        LOG_ERROR("AUDIO: Could not play sound! " << Mix_GetError());
    }
}

Mix_Music* LoadMusic(const char* filename) {
    auto music = Mix_LoadMUS(filename);
    if (!music) {
        LOG_ERROR("Failed to load music: " << filename << " -- " << Mix_GetError());
    }
    return music;
}

void PlayMusic(Mix_Music* music, U32 loops) {
    if (AUDIO_DISABLED) return;
    if (Mix_PlayMusic(music, loops) == -1) {
        LOG_ERROR("Couldnt not play music!" << Mix_GetError());
    }
}
#endif