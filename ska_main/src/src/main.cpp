#define SDL_MAIN_HANDLED
#include "__internalConfig/LoggerConfig.h"
#include "GameLibrary.h"

namespace ska {
	class TerminateProcessException;
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	const auto& gameLib = ska::GameLibrary::get();

	if (!gameLib.isLoaded()) {
		SLOG_STATIC(ska::LogLevel::Error, ska::GameLibrary) << "Unable to load the game library, aborting process.";
		return -1;
	}

	gameLib.init();

	const int result = gameLib.run();

	return result;
}
