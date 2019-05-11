#include <iostream>
#include "Music.h"
#include "SoundRenderer.h"
#include "Logging/Logger.h"
#include "Core/SDLLibrary.h"
#include "SDLMixerLibrary.h"

ska::SoundRenderer::SoundRenderer(ska::GameEventDispatcher& ged) :
	ska::SubObserver<SoundEvent>(std::bind(&SoundRenderer::handleSoundEvent, this, std::placeholders::_1), ged),
	ska::SubObserver<WorldEvent>(std::bind(&SoundRenderer::handleWorldEvent, this, std::placeholders::_1), ged),
	m_currentPlayed(nullptr) {

	if(SDLMixerLibrary::get().allocateChannels(10) != static_cast<int>(10)) {
		SLOG(LogLevel::Error) << "SoundManager error : " << ska::SDLLibrary::get().getError();
	}
}

void ska::SoundRenderer::play(Mix_Music* m_instance) {
	SDLMixerLibrary::get().pauseMusic();
	if(SDLMixerLibrary::get().playMusic(m_instance, -1) == -1) {
		SLOG(LogLevel::Error) << "SoundManager error : " << ska::SDLLibrary::get().getError();
		m_currentPlayed = nullptr;
	} else {
		m_currentPlayed = m_instance;
	}
}

void ska::SoundRenderer::setMusicVolume(float volPcts) const{
	if (m_currentPlayed != nullptr) {
		SDLMixerLibrary::get().volumeMusic(static_cast<int>((volPcts * MIX_MAX_VOLUME) / 100));
	}
}

bool ska::SoundRenderer::handleSoundEvent(SoundEvent& se) {
	auto music = se.getMusic();
	if (music != nullptr && se.getEventType() == SoundEventType::PLAY_MUSIC) {
		music->play(*this);
	}
	return false;
}

bool ska::SoundRenderer::handleWorldEvent(WorldEvent& we) {
	auto music = we.bgm;
	if (music != nullptr && (we.type == WorldEventType::WORLD_CREATE ||
		we.type == WorldEventType::WORLD_CHANGE)) {
		music->play(*this);
	}
	return false;
}
