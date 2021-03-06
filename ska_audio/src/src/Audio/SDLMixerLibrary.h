#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <gsl/pointers>

namespace ska {

	template<int Id>
	struct MixerIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	enum class MixerCalls {
		SDL_MIXER_INIT,
		SDL_MIXER_OPEN_AUDIO,
		SDL_MIXER_CLOSE_AUDIO,
		SDL_MIXER_QUIT,
		SDL_MIXER_ALLOCATE_CHANNELS,
		SDL_MIXER_LOAD_MUSIC,
		SDL_MIXER_PLAY_MUSIC,
		SDL_MIXER_PAUSE_MUSIC,
		SDL_MIXER_VOLUME_MUSIC,
		SDL_MIXER_FREE_MUSIC,
		SDL_MIXER_WAV_RW,
		SDL_MIXER_FREE_CHUNK
	};

#define SKA_MIXER_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(MixerIdNamedFunction, static_cast<int>(MixerCalls::ENUM), FUNCTION)

	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_INIT, int(int));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_OPEN_AUDIO, int(int, Uint16, int, int));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_CLOSE_AUDIO, void());
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_QUIT, void());
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_ALLOCATE_CHANNELS, int(int));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_LOAD_MUSIC, Mix_Music*(const char*));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_PLAY_MUSIC, int(Mix_Music *, int));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_PAUSE_MUSIC, void());
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_VOLUME_MUSIC, int(int));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_FREE_MUSIC, void(Mix_Music*));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_WAV_RW, Mix_Chunk*(SDL_RWops* src, int freesrc));
	SKA_MIXER_LIB_CALLS_DEFINE(SDL_MIXER_FREE_CHUNK, void(Mix_Chunk*));

#define SKA_MIXER_DYN_LIB_NAME_ENTRY(ENUM) MixerIdNamedFunction<static_cast<int>(MixerCalls::ENUM)>

	using SDLMixerDynLib = ska::DynamicLibrary <
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_INIT),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_OPEN_AUDIO),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_CLOSE_AUDIO),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_QUIT),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_ALLOCATE_CHANNELS),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_LOAD_MUSIC),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_PLAY_MUSIC),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_PAUSE_MUSIC),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_VOLUME_MUSIC),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_FREE_MUSIC),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_WAV_RW),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(SDL_MIXER_FREE_CHUNK)
	>;


	class SDLMixerLibrary : public SDLMixerDynLib {
	#define callMixer(enumIndex) call<SKA_MIXER_DYN_LIB_NAME_ENTRY(enumIndex)>
	public:
		inline int init(int flags) const {
			return callMixer(SDL_MIXER_INIT)(flags);
		}

		inline int openAudio(int frequency, Uint16 format, int channels, int chunksize) const {
			return callMixer(SDL_MIXER_OPEN_AUDIO)(frequency, format, channels, chunksize);
		}

		inline int allocateChannels(int numchans) const {
			return callMixer(SDL_MIXER_ALLOCATE_CHANNELS)(numchans);
		}

		inline int playMusic(Mix_Music* music, int loops) const {
			return callMixer(SDL_MIXER_PLAY_MUSIC)(music, loops);
		}

		inline void pauseMusic() const {
			callMixer(SDL_MIXER_PAUSE_MUSIC)();
		}

		inline int volumeMusic(int volume) const {
			return callMixer(SDL_MIXER_VOLUME_MUSIC)(volume);
		}

		inline Mix_Music* loadMusic(const std::string& file) const {
			return callMixer(SDL_MIXER_LOAD_MUSIC)(file.c_str());
		}

		inline void quit() const {
			callMixer(SDL_MIXER_QUIT)();
		}

		inline void closeAudio() const {
			callMixer(SDL_MIXER_CLOSE_AUDIO)();
		}

		inline void freeMusic(Mix_Music* music) const {
			callMixer(SDL_MIXER_FREE_MUSIC)(music);
		}

		inline Mix_Chunk* loadWavRW(SDL_RWops* src, int freesrc) const {
			return callMixer(SDL_MIXER_WAV_RW)(src, freesrc);
		}

		inline void freeChunk(Mix_Chunk* chunk) const {
			callMixer(SDL_MIXER_FREE_CHUNK)(chunk);
		}

		static const SDLMixerLibrary& get();

	private:
		SDLMixerLibrary() : SDLMixerDynLib("SDL2_mixer") {}
	#undef callMixer
	};
}
