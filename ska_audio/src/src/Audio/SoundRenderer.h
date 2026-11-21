#pragma once
#include "Base/Patterns/Observer.h"
#include "Core/Data/Events/WorldEvent.h"
#include "Core/Data/Events/SoundEvent.h"
#include "Core/Data/Events/EventDispatcher.h"
#include "Core/Data/Events/GameEventDispatcher.h"
#include "Base/Patterns/SubObserver.h"

extern "C" {
	typedef struct MIX_Audio MIX_Audio;
}

namespace ska {

	class SoundRenderer :
		public SubObserver<SoundEvent>,
		public SubObserver<WorldEvent> {
	public:
		explicit SoundRenderer(GameEventDispatcher& em);
		void play(MIX_Audio* m_instance);
		void setMusicVolume(float volPcts) const;
		bool handleSoundEvent(SoundEvent& se);
		bool handleWorldEvent(WorldEvent& we);
		virtual ~SoundRenderer() = default;

	private:
		MIX_Audio* m_currentPlayed;
	};

}
