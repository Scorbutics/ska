#include "Audio/__internalConfig/LoggerConfig.h"
#include "SDLSound.h"
#include "SDLMixerLibrary.h"
#include "Core/SDLLibrary.h"

ska::SDLSound::SDLSound(const std::string& soundName) : m_instance(nullptr) {
	m_instance = SDLMixerLibrary::get().loadWavRW(SDLLibrary::get().rwFromFile(soundName, "rb"), 1);
}

ska::SDLSound::~SDLSound() {
	SDLMixerLibrary::get().freeChunk(m_instance);
}
