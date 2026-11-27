#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"
#include <array>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <gsl/pointers>

namespace ska {
	template<int Id>
	struct MixerIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	// The SDL3_mixer functions used by the engine, loaded at run time (libSDL3_mixer)
	enum class MixerCalls {
		MIX_INIT,
		MIX_QUIT,
		MIX_CREATE_MIXER_DEVICE,
		MIX_DESTROY_MIXER,
		MIX_LOAD_AUDIO,
		MIX_DESTROY_AUDIO,
		MIX_CREATE_TRACK,
		MIX_DESTROY_TRACK,
		MIX_SET_TRACK_AUDIO,
		MIX_SET_TRACK_LOOPS,
		MIX_PLAY_TRACK,
		MIX_STOP_TRACK,
		MIX_SET_TRACK_GAIN,
		MIX_PLAY_AUDIO,
		MIX_SET_MASTER_GAIN,
		MIX_TRACK_PLAYING
	};

#define SKA_MIXER_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(MixerIdNamedFunction, static_cast<int>(MixerCalls::ENUM), FUNCTION)

	SKA_MIXER_LIB_CALLS_DEFINE(MIX_INIT, bool());
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_QUIT, void());
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_CREATE_MIXER_DEVICE, MIX_Mixer*(SDL_AudioDeviceID, const SDL_AudioSpec*));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_DESTROY_MIXER, void(MIX_Mixer*));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_LOAD_AUDIO, MIX_Audio*(MIX_Mixer*, const char*, bool));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_DESTROY_AUDIO, void(MIX_Audio*));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_CREATE_TRACK, MIX_Track*(MIX_Mixer*));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_DESTROY_TRACK, void(MIX_Track*));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_SET_TRACK_AUDIO, bool(MIX_Track*, MIX_Audio*));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_SET_TRACK_LOOPS, bool(MIX_Track*, int));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_PLAY_TRACK, bool(MIX_Track*, SDL_PropertiesID));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_STOP_TRACK, bool(MIX_Track*, Sint64));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_SET_TRACK_GAIN, bool(MIX_Track*, float));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_PLAY_AUDIO, bool(MIX_Mixer*, MIX_Audio*));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_SET_MASTER_GAIN, bool(MIX_Mixer*, float));
	SKA_MIXER_LIB_CALLS_DEFINE(MIX_TRACK_PLAYING, bool(MIX_Track*));

	#define SKA_MIXER_DYN_LIB_NAME_ENTRY(ENUM) MixerIdNamedFunction<static_cast<int>(MixerCalls::ENUM)>

	using SDLMixerDynLib = DynamicLibrary <
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_INIT),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_QUIT),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_CREATE_MIXER_DEVICE),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_DESTROY_MIXER),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_LOAD_AUDIO),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_DESTROY_AUDIO),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_CREATE_TRACK),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_DESTROY_TRACK),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_SET_TRACK_AUDIO),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_SET_TRACK_LOOPS),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_PLAY_TRACK),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_STOP_TRACK),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_SET_TRACK_GAIN),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_PLAY_AUDIO),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_SET_MASTER_GAIN),
		SKA_MIXER_DYN_LIB_NAME_ENTRY(MIX_TRACK_PLAYING)
	>;

	// The audio of the engine (SDL3_mixer) : a mixer on the default audio device (its gain : the master volume), a track
	// playing the music (its gain : the volume of the music), tracks for the sounds played at once (their gain : the
	// volume of the sounds). The volumes stay, whatever is played next
	class SDLMixerLibrary : public SDLMixerDynLib {
	#define callMixer(enumIndex) call<SKA_MIXER_DYN_LIB_NAME_ENTRY(enumIndex)>
	public:
		// 0 on success
		inline int openAudio() const {
			if (!callMixer(MIX_INIT)()) {
				return -1;
			}
			m_mixer = callMixer(MIX_CREATE_MIXER_DEVICE)(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
			if (m_mixer == nullptr) {
				return -1;
			}
			m_music = callMixer(MIX_CREATE_TRACK)(m_mixer);
			if (m_music == nullptr) {
				return -1;
			}
			for (auto& track : m_sounds) {
				track = callMixer(MIX_CREATE_TRACK)(m_mixer);
			}
			callMixer(MIX_SET_MASTER_GAIN)(m_mixer, m_masterGain);
			callMixer(MIX_SET_TRACK_GAIN)(m_music, m_musicGain);
			return 0;
		}

		inline void closeAudio() const {
			for (auto& track : m_sounds) {
				if (track != nullptr) {
					callMixer(MIX_DESTROY_TRACK)(track);
					track = nullptr;
				}
			}
			if (m_music != nullptr) {
				callMixer(MIX_DESTROY_TRACK)(m_music);
				m_music = nullptr;
			}
			if (m_mixer != nullptr) {
				callMixer(MIX_DESTROY_MIXER)(m_mixer);
				m_mixer = nullptr;
			}
		}

		inline void quit() const {
			callMixer(MIX_QUIT)();
		}

		// A music, read while it is played (nullptr : no audio device, or an error)
		inline MIX_Audio* loadMusic(const std::string& file) const {
			return m_mixer == nullptr ? nullptr : callMixer(MIX_LOAD_AUDIO)(m_mixer, file.c_str(), false);
		}

		// A sound, decoded when it is loaded
		inline MIX_Audio* loadSound(const std::string& file) const {
			return m_mixer == nullptr ? nullptr : callMixer(MIX_LOAD_AUDIO)(m_mixer, file.c_str(), true);
		}

		inline void freeAudio(MIX_Audio* audio) const {
			if (audio != nullptr) {
				callMixer(MIX_DESTROY_AUDIO)(audio);
			}
		}

		// "loops" : -1 for ever. 0 on success
		inline int playMusic(MIX_Audio* music, int loops) const {
			if (m_music == nullptr || music == nullptr || !callMixer(MIX_SET_TRACK_AUDIO)(m_music, music)) {
				return -1;
			}
			callMixer(MIX_SET_TRACK_LOOPS)(m_music, loops);
			return callMixer(MIX_PLAY_TRACK)(m_music, 0) ? 0 : -1;
		}

		inline void stopMusic() const {
			if (m_music != nullptr) {
				callMixer(MIX_STOP_TRACK)(m_music, 0);
			}
		}

		// The volumes, from 0 to 1
		inline void setMasterVolume(float volume) const {
			m_masterGain = volume;
			if (m_mixer != nullptr) {
				callMixer(MIX_SET_MASTER_GAIN)(m_mixer, volume);
			}
		}

		inline void setMusicVolume(float volume) const {
			m_musicGain = volume;
			if (m_music != nullptr) {
				callMixer(MIX_SET_TRACK_GAIN)(m_music, volume);
			}
		}

		inline void setSoundVolume(float volume) const {
			m_soundGain = volume;
		}

		// A sound played once, over the music and the other sounds (a free track of the sounds, else the next one : the
		// oldest sound stops). 0 on success
		inline int playSound(MIX_Audio* sound) const {
			if (m_mixer == nullptr || sound == nullptr) {
				return -1;
			}
			auto* track = static_cast<MIX_Track*>(nullptr);
			for (auto* candidate : m_sounds) {
				if (candidate != nullptr && !callMixer(MIX_TRACK_PLAYING)(candidate)) {
					track = candidate;
					break;
				}
			}
			if (track == nullptr) {
				track = m_sounds[m_nextSound];
				m_nextSound = (m_nextSound + 1) % m_sounds.size();
			}
			if (track == nullptr || !callMixer(MIX_SET_TRACK_AUDIO)(track, sound)) {
				return -1;
			}
			callMixer(MIX_SET_TRACK_GAIN)(track, m_soundGain);
			return callMixer(MIX_PLAY_TRACK)(track, 0) ? 0 : -1;
		}

		static const SDLMixerLibrary& get();

	private:
		SDLMixerLibrary() : SDLMixerDynLib("SDL3_mixer") {}
		mutable MIX_Mixer* m_mixer = nullptr;
		mutable MIX_Track* m_music = nullptr;
		mutable std::array<MIX_Track*, 12> m_sounds{};
		mutable std::size_t m_nextSound = 0;
		mutable float m_masterGain = 1.F;
		mutable float m_musicGain = 1.F;
		mutable float m_soundGain = 1.F;
	#undef callMixer
	};
}
