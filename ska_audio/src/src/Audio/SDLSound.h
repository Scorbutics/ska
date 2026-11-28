#pragma once
#include <string>

extern "C" {
	typedef struct MIX_Audio MIX_Audio;
}

namespace ska {
	class SDLSound {
	public:
		SDLSound(const std::string& soundName);
		SDLSound(const SDLSound&) = delete;
		SDLSound& operator=(const SDLSound&) = delete;
		virtual ~SDLSound();

		// Played once (several times at once when asked again before its end) ; nothing without an audio device. "gain" : its
		// volume (0 .. 1), times the volume of the sounds
		void play(float gain = 1.F) const;
		bool loaded() const { return m_instance != nullptr; }

	private:
		MIX_Audio* m_instance;
	};
}
