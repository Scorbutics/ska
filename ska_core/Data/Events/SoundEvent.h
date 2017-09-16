#pragma once

namespace ska {
	class Music;
	enum SoundEventType {
		PLAY_MUSIC,
		PLAY_SOUND,
		STOP_MUSIC,
		STOP_SOUND
	};

	/**
	* \brief Declares an event struct that is sent to observers when playing / stopping a sound or a music is necessary
	*/
	class SoundEvent {
	public:
		SoundEvent(Music* music, SoundEventType set);
		virtual ~SoundEvent() = default;
		Music* getMusic();
		const SoundEventType& getEventType() const;

	private:
		Music* m_music;
		const SoundEventType m_eventType;

	};
}
