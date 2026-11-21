#include "LoggerConfig.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

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
	
	initLibraries();

	resultCode = context.run();

	if (context.shouldExit() || resultCode != 0) {
		return(-1);
	}

	return resultCode;
}
