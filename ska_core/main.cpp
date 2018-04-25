#define SDL_MAIN_HANDLED
#include "GameApp.h"
#include <SDL_main.h>

namespace ska {
	class TerminateProcessException;
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	auto gameApp = ska::GameApp::instantiate();

	try {
		gameApp->run();
	} catch (ska::TerminateProcessException& tpe) {
		return gameApp->onTerminate(tpe);
	} catch (ska::GenericException& e) {
		return gameApp->onException(e);
	}

	return 0;
}
