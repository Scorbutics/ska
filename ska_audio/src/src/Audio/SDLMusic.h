#pragma once
#include <string>

extern "C" {
	typedef struct MIX_Audio MIX_Audio;
}

namespace ska {
	class SDLMusic {
		friend class Music;
	public:
		SDLMusic(const std::string& musicPath);
		void setVolume(float vol);
		virtual ~SDLMusic();

	private:
		MIX_Audio* m_instance;
		float m_volume;
	};
}
