#include "LoggerConfig.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <iostream>

#include "Core/SDLLibrary.h"
#include "Graphic/SDLTTFLibrary.h"
#include "Graphic/SDLImageLibrary.h"

struct GuiTestMain;

void initLibraries() {
	ska::SDLLibrary::get().setMainReady();

	if (ska::SDLLibrary::get().init(SDL_INIT_VIDEO) < 0) {
		throw std::runtime_error("Erreur lors de l'initialisation de la SDL : " + std::string(ska::SDLLibrary::get().getError()));
	}

	/* Fix GDB Bug with named thread on windows (Mixer raises an exception when init) */
	if (!ska::SDLLibrary::get().setHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1")) {
		SLOG_STATIC(ska::LogLevel::Warn, GuiTestMain) << "Attention : Windows nomme actuellement les threads créés par l'application alors que le programme tente de désactiver cette fonctionnalité.";
	}

	if (!ska::SDLLibrary::get().setHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
		SLOG_STATIC(ska::LogLevel::Warn, GuiTestMain) << "Attention : Linear texture filtering impossible à activer.";
	}

	if (!(ska::SDLImageLibrary::get().init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		throw std::runtime_error("Erreur lors de l'initialisation de SDL_image : " + std::string(ska::SDLLibrary::get().getError()));
	}

	if (ska::SDLTTFLibrary::get().init() == -1) {
		SLOG_STATIC(ska::LogLevel::Error, GuiTestMain) << "Erreur d'initialisation de TTF_Init : " << ska::SDLLibrary::get().getError();
	}
}
#undef main
int main(int argc, char* argv[]) {
	doctest::Context context = doctest::Context{argc, argv};
	auto resultCode = 0;

	// sort the test cases by their name
	context.setOption("order-by", "name");
	context.setOption("success", "true");
	
	//initLibraries();

	resultCode = context.run();

	if (context.shouldExit() || resultCode != 0) {
		return(-1);
	}

	return resultCode;
}
