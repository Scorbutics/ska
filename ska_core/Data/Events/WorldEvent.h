#pragma once

namespace ska {

	class Music;
	enum WorldEventType {
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
			m_type(wet), 
			m_bgm(nullptr){
		}

		void setBgm(Music& m) {
			m_bgm = &m;
		}

		Music* getBgm() const {
			return m_bgm;
		}

		const WorldEventType& getEventType() const {
			return m_type;
		}

		virtual ~WorldEvent() = default;

	private:
		WorldEventType m_type;
		Music* m_bgm;
	};

}
