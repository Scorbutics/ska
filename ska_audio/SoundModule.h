#pragma once
#include <string>
#include "Module.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {

    template <class SoundManager>
	class SoundModule :
        public ska::Module {

	public:
		explicit SoundModule(const std::string& moduleName, GameEventDispatcher& ged) :
			Module(moduleName),
            m_soundManager(ged) {
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
                SKA_LOG_ERROR("Impossible d'initialiser SDL_mixer : ", Mix_GetError());
            }
        }

		~SoundModule() override {
			Mix_CloseAudio();
            Mix_Quit();
        }

    private:
        SoundManager m_soundManager;
	};
}
