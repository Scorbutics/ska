#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"

namespace ska {

	template<int Id>
	struct GameIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	enum class GameCalls {
		GAME_INIT,
		GAME_RUN
	};

#define SKA_GAME_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(GameIdNamedFunction, static_cast<int>(GameCalls::ENUM), FUNCTION)

	SKA_GAME_LIB_CALLS_DEFINE(GAME_INIT, void(void));
	SKA_GAME_LIB_CALLS_DEFINE(GAME_RUN, int(void));

#define SKA_GAME_DYN_LIB_NAME_ENTRY(ENUM) GameIdNamedFunction<static_cast<int>(GameCalls::ENUM)>

	using GameDynLib = DynamicLibrary <
		SKA_GAME_DYN_LIB_NAME_ENTRY(GAME_INIT),
		SKA_GAME_DYN_LIB_NAME_ENTRY(GAME_RUN)
	> ;

	class GameLibrary : public GameDynLib {
#define callGame(enumIndex) call<SKA_GAME_DYN_LIB_NAME_ENTRY(enumIndex)>

	public:
		inline int run() const { return callGame(GAME_RUN)(); }
		inline void init() const { callGame(GAME_INIT)(); }

		static GameLibrary& get();

	private:
		GameLibrary() : GameDynLib("Game") {}

#undef callGame
	};
}
