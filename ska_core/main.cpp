#ifndef __ANDROID__
#include <SDL_main.h>
//We need this include to define "main" as "SDL_main"

#include "GameApp.h"

namespace ska {
	class TerminateProcessException;
}

#ifdef __cplusplus
extern "C"
#endif
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
#endif
