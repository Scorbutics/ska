#include <SDL_mixer.h>

#include "SoundModule.h"
#include "Logging/Logger.h"

ska::SoundModule::SoundModule(const std::string& moduleName) :
	Module(moduleName) {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
		SKA_LOG_ERROR("Impossible d'initialiser SDL_mixer : ", Mix_GetError());
	}
}

ska::SoundModule::~SoundModule() {
	Mix_CloseAudio();
	Mix_Quit();
}
