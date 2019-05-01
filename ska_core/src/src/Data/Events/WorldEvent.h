#pragma once

namespace ska {

	class Music;

	enum class WorldEventType {
		WORLD_CREATE,
		WORLD_CHANGE,
		WORLD_QUIT
	};

	/**
	* \brief Declares an event struct that is sent to observers when the world is altered (world creation, world replacement ...)
	*/
	class WorldEvent {
	public:
		explicit WorldEvent(WorldEventType wet) :
			type(wet),
			bgm(nullptr){
		}

		explicit WorldEvent(WorldEventType wet, Music& currentBgm) :
			type(wet),
			bgm(&currentBgm){
		}

		const WorldEventType type;
		Music* const bgm;
        unsigned int blocksWidth {};
        unsigned int blocksHeight {};
        unsigned int blockSize {};
        std::string fullName;
	};

}
