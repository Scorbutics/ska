#include "Audio/__internalConfig/LoggerConfig.h"
#include "SDLSound.h"
#include "SDLMixerLibrary.h"

ska::SDLSound::SDLSound(const std::string& soundName) : m_instance(nullptr) {
	m_instance = SDLMixerLibrary::get().loadSound(soundName);
}

ska::SDLSound::~SDLSound() {
	SDLMixerLibrary::get().freeAudio(m_instance);
}

void ska::SDLSound::play(float gain) const {
	SDLMixerLibrary::get().playSound(m_instance, gain);
}
