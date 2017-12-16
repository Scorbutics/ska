#include "Game.h"
#include "Inputs/KeyboardInputMapContext.h"
#include "Inputs/KeyboardInputGUIContext.h"
#include "Graphic/SDLFont.h"
#include "StateSandbox.h"
#include "Exceptions/FileException.h"
#include "Graphic/SDLRenderer.h"
#include "Core/Window.h"
#include "Exceptions/IllegalStateException.h"
#include "CoreModule.h"
#include "GraphicModule.h"

namespace ska {
	class FileException;
}

std::unique_ptr<ska::GameApp> ska::GameApp::get() {
	ska::GameConfiguration gc;
	gc.requireModule<CoreModule>("Core");
	gc.requireModule<GraphicModule>("Graphics");

	auto widthBlocks = 30;
	auto heightBlocks = 20;

	std::string title = "Default title";
	try {
		IniReader reader("gamesettings.ini");
		widthBlocks = reader.get<int>("Window width_blocks");
		heightBlocks = reader.get<int>("Window height_blocks");
		title = reader.get<std::string>("Window title");
	} catch (FileException& fe) {
		std::cerr << "Error while loading game settings : " << fe.what() << std::endl;
	}

	static constexpr auto tailleblocFenetre = 32;
	auto window = std::make_unique<Window>("ska physics", widthBlocks * tailleblocFenetre, heightBlocks * tailleblocFenetre);
	auto renderer = std::make_unique<SDLRenderer>(*window, -1, SDL_RENDERER_ACCELERATED);

	return std::make_unique<Game>(std::move(gc), std::move(renderer), std::move(window));
}

void LogsConfiguration() {
	ska::LoggerFactory::staticAccess<ska::WorldCollisionResponse>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
}

Game::Game(ska::GameConfiguration&& gc, RendererPtr&& renderer, WindowPtr&& window) :
	GameCore(std::forward<ska::GameConfiguration>(gc), std::forward<RendererPtr>(renderer), std::forward<WindowPtr>(window)) {

	/* Configure inputs types */
	addInputContext<ska::KeyboardInputMapContext>(ska::EnumContextManager::CONTEXT_MAP);
	addInputContext<ska::KeyboardInputGUIContext>(ska::EnumContextManager::CONTEXT_GUI);

	LogsConfiguration();

	ska::SDLFont::DEFAULT_FONT_FILE = "." FILE_SEPARATOR "Resources" FILE_SEPARATOR "Fonts" FILE_SEPARATOR "FiraSans-Medium.ttf";
	navigateToState<StateSandbox>(m_eventDispatcher);
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
