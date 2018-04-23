#include <iostream>
#include <SDL_mixer.h>
#include "Music.h"
#include "SoundRenderer.h"
#include "Logging/Logger.h"

ska::SoundRenderer::SoundRenderer(ska::GameEventDispatcher& ged) :
	ska::SubObserver<SoundEvent>(std::bind(&SoundRenderer::handleSoundEvent, this, std::placeholders::_1), ged),
	ska::SubObserver<WorldEvent>(std::bind(&SoundRenderer::handleWorldEvent, this, std::placeholders::_1), ged),
	m_currentPlayed(nullptr) {

	if(Mix_AllocateChannels(10) != static_cast<int>(10)) {
		SKA_LOG_ERROR("SoundManager error : ", Mix_GetError());
	}
}

void ska::SoundRenderer::play(Mix_Music* m_instance) {
	Mix_PauseMusic();
	if(Mix_PlayMusic(m_instance, -1) == -1) {
		SKA_LOG_ERROR("SoundManager error : ", Mix_GetError());
		m_currentPlayed = nullptr;
	} else {
		m_currentPlayed = m_instance;
	}
}

void ska::SoundRenderer::setMusicVolume(float volPcts) const{
	if (m_currentPlayed != nullptr) {
		Mix_VolumeMusic(static_cast<int>((volPcts * MIX_MAX_VOLUME) / 100));
	}
}

bool ska::SoundRenderer::handleSoundEvent(SoundEvent& se) {
	auto music = se.getMusic();
	if (se.getEventType() == SoundEventType::PLAY_MUSIC) {
		music->play(*this);
	}
	return false;
}

bool ska::SoundRenderer::handleWorldEvent(WorldEvent& we) {
	auto music = we.getBgm();
	if (we.getEventType() == WorldEventType::WORLD_CREATE ||
		we.getEventType() == WorldEventType::WORLD_CHANGE) {
		music->play(*this);
	}
	return false;
}
