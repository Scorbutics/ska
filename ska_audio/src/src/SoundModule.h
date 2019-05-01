#pragma once
#include <string>
#include "Data/Events/GameEventDispatcher.h"
#include "SDLLibrary.h"
#include "Audio/SDLMixerLibrary.h"

namespace ska {

    template <class SoundManager>
	class SoundModule {

	public:
		template <class SM>
		explicit SoundModule(const std::string& moduleName, SM&& ged) :
            m_soundManager(std::forward<SM>(ged)) {
            if (SDLMixerLibrary::get().openAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
                SKA_LOG_ERROR("Impossible d'initialiser SDL_mixer : ", ska::SDLLibrary::get().getError());
            }
        }

		void eventUpdate(unsigned int){
		}

		virtual ~SoundModule() {
			SDLMixerLibrary::get().closeAudio();
            SDLMixerLibrary::get().quit();
        }

    private:
        SoundManager m_soundManager;
	};
}
