#pragma once

namespace ska {
	enum GameLibrariesInitializedEventType {
		ALL
	};

	/**
	* \brief Declares an event struct that is sent to observers when a game library is ready
	*/
	class GameLibrariesInitializedEvent {
	public:
		explicit GameLibrariesInitializedEvent(GameLibrariesInitializedEventType glit) : m_eventType(glit) {
		}
		virtual ~GameLibrariesInitializedEvent() = default;

		const GameLibrariesInitializedEventType& getEventType() const {
            return m_eventType;
		}

	private:
		const GameLibrariesInitializedEventType m_eventType;

	};
}
