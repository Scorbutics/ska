#include "SDLMixerLibrary.h"
#include "SDLMusic.h"

ska::SDLMusic::SDLMusic(const std::string& musicPath) : m_volume(-1) {
	m_instance = SDLMixerLibrary::get().loadMusic(musicPath.c_str());
}

void ska::SDLMusic::setVolume(float vol) {
	m_volume = vol;
}

ska::SDLMusic::~SDLMusic() {
	SDLMixerLibrary::get().freeMusic(m_instance);
}
