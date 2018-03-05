#pragma once
#include <unordered_set>

namespace ska {
	enum class GameEventType {
		GAME_WINDOW_READY,
		GAME_WINDOW_RESIZED
	};

	/**
	* \brief Declares an event struct that is sent to observers when a global game event happens
	*/
	class GameEvent {
	public:
		explicit GameEvent(GameEventType get) :
			windowWidth(0),
			windowHeight(0),
			m_eventType(get) {
		}

		virtual ~GameEvent() = default;
		const GameEventType& getEventType() const{
			return m_eventType;
		}


		unsigned int windowWidth;
		unsigned int windowHeight;
		std::unordered_set<ska::EntityId> entitiesRemoved;

	private:
		const GameEventType m_eventType;

	};
}
