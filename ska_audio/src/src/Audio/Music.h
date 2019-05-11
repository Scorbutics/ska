#pragma once

#include "Core/Utils/ManagedResource.h"
#include "SDLMusic.h"

namespace ska {
	class SoundRenderer;

	class Music : 
		public ManagedResource<SDLMusic, std::string> {
	public:
		explicit Music(const std::string& musicName);
		void setVolume(float vol) const;
		void play(SoundRenderer& sndMgr) const;
		virtual ~Music() = default;
	};
}
