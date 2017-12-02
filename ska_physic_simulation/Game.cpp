#include "Game.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "Draw/SDLFont.h"
#include "Utils/SkaConstants.h"
#include "StateSandbox.h"

std::unique_ptr<ska::GameApp> ska::GameApp::get() {
	std::unique_ptr<Game> wgc = std::make_unique<Game>();
	wgc->init();
	return std::move(wgc);
}

void LogsConfiguration() {
	ska::LoggerFactory::staticAccess<ska::WorldCollisionResponse>().configureLogLevel(ska::EnumLogLevel::DISABLED);
}

void Game::init() {
	/* Configure inputs types */
	addInputContext<ska::KeyboardInputMapContext>(ska::EnumContextManager::CONTEXT_MAP);
	addInputContext<ska::KeyboardInputGUIContext>(ska::EnumContextManager::CONTEXT_GUI);

	LogsConfiguration();

	ska::SDLFont::DEFAULT_FONT_FILE = "." FILE_SEPARATOR "Resources" FILE_SEPARATOR "Fonts" FILE_SEPARATOR "FiraSans-Medium.ttf";
	navigateToState<StateSandbox>();
}

int Game::onTerminate(ska::TerminateProcessException& tpe) {
	SKA_LOG_MESSAGE(tpe.what());
	return 0;
}

int Game::onException(ska::GenericException& e) {
	/* Handles Generics Game exceptions */
	std::cerr << e.what() << std::endl;
	return 0;
}
