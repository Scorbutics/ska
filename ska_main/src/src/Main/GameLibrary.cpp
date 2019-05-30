#include "__internalConfig/LoggerConfig.h"
#include "GameLibrary.h"

#define SKA_GAME_LIB_NAME_DEFINE(ENUM, NAME) \
	SKA_LIB_NAME_DEFINE(ska::GameIdNamedFunction, static_cast<int>(ska::GameCalls::ENUM), NAME)

SKA_GAME_LIB_NAME_DEFINE(GAME_RUN, "run");
SKA_GAME_LIB_NAME_DEFINE(GAME_INIT, "init");

ska::GameLibrary& ska::GameLibrary::get() {
	static GameLibrary instance;
	return instance;	
}
