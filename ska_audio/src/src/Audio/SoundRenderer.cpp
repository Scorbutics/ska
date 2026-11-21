#include "Audio/__internalConfig/LoggerConfig.h"
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
}

void ska::SoundRenderer::play(MIX_Audio* m_instance) {
	SDLMixerLibrary::get().stopMusic();
	if(SDLMixerLibrary::get().playMusic(m_instance, -1) != 0) {
		SLOG(LogLevel::Error) << "SoundManager error : " << ska::SDLLibrary::get().getError();
		m_currentPlayed = nullptr;
	} else {
		m_currentPlayed = m_instance;
	}
}

void ska::SoundRenderer::setMusicVolume(float volPcts) const{
	if (m_currentPlayed != nullptr) {
		SDLMixerLibrary::get().setMusicVolume(volPcts / 100.F);
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
