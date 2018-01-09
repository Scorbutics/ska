#pragma once
#include "Utils/Observer.h"
#include "Data/Events/WorldEvent.h"
#include "Data/Events/SoundEvent.h"
#include "Data/Events/EventDispatcher.h"
#include "Data/Events/GameEventDispatcher.h"
#include "Utils/SubObserver.h"

extern "C" {
    struct _Mix_Music;
    typedef _Mix_Music Mix_Music;
}

namespace ska {

	class SoundRenderer :
		public SubObserver<SoundEvent>,
		public SubObserver<WorldEvent> {
	public:
		explicit SoundRenderer(GameEventDispatcher& em);
		void play(Mix_Music* m_instance);
		void setMusicVolume(float volPcts) const;
		bool handleSoundEvent(SoundEvent& se);
		bool handleWorldEvent(WorldEvent& we);
		virtual ~SoundRenderer() = default;

	private:
		Mix_Music* m_currentPlayed;
	};

}
