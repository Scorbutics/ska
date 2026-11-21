#pragma once
#include <string>

extern "C" {
	typedef struct MIX_Audio MIX_Audio;
}

namespace ska {
	class SDLSound {
	public:
		SDLSound(const std::string& soundName);
		virtual ~SDLSound();

	private:
		MIX_Audio* m_instance;
	};
}
