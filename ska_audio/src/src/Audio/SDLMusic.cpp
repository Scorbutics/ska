#include "Audio/__internalConfig/LoggerConfig.h"
#include "SDLMixerLibrary.h"
#include "SDLMusic.h"

ska::SDLMusic::SDLMusic(const std::string& musicPath) : m_volume(-1) {
	m_instance = SDLMixerLibrary::get().loadMusic(musicPath);
}

void ska::SDLMusic::setVolume(float vol) {
	m_volume = vol;
}

ska::SDLMusic::~SDLMusic() {
	SDLMixerLibrary::get().freeAudio(m_instance);
}
