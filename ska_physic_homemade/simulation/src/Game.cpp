#include <iostream>
#include "LoggerConfig.h"
#include "Game.h"
#include "World/Inputs/KeyboardInputMapContext.h"
#include "World/Inputs/KeyboardInputGUIContext.h"
#include "Core/Draw/VectorDrawableContainer.h"
#include "Graphic/SDLFont.h"
#include "StateSandbox.h"
#include "Core/Exceptions/FileException.h"
#include "Graphic/SDLRenderer.h"
#include "Graphic/SDLWindow.h"
#include "Core/Exceptions/IllegalStateException.h"
#include "CoreModule.h"
#include "GraphicModule.h"
#include "Base/IO/Readers/IniReader.h"

std::string ska::SDLFont::DEFAULT_FONT_FILE = "Fonts/FiraSans-Medium.ttf";

namespace ska {
	class FileException;


	std::unique_ptr<ska::GameApp> GameAppGet() {
		auto widthBlocks = 30;
		auto heightBlocks = 20;

		std::string title = "Default title";
		try {
			IniReader reader("gamesettings.ini");
			widthBlocks = reader.get<int>("Window width_blocks");
			heightBlocks = reader.get<int>("Window height_blocks");
			title = reader.get<std::string>("Window title");
		}
		catch (FileException& fe) {
			std::cerr << "Error while loading game settings : " << fe.what() << std::endl;
		}

		static constexpr auto tailleblocFenetre = 32;
		auto window = std::make_unique<SDLWindow>("ska physics", widthBlocks * tailleblocFenetre, heightBlocks * tailleblocFenetre);
		auto renderer = std::make_unique<SDLRenderer>(*window, -1, SDL_RENDERER_ACCELERATED);
		auto dc = std::make_unique<ska::VectorDrawableContainer>(*renderer);

		using GameConf = ska::GameConfiguration<ska::ExtensibleGameEventDispatcher<>, CoreModule<ska::EntityManager>, GraphicModule>;
		auto gc = std::make_unique<GameConf>();

		auto em = ska::EntityManager{gc->getEventDispatcher()};
		auto& core = gc->requireModule<CoreModule<ska::EntityManager>>("Core", std::move(em), gc->getEventDispatcher());

		/* Configure inputs types */
		core.addInputContext<ska::KeyboardInputMapContext>(ska::EnumContextManager::CONTEXT_MAP);
		core.addInputContext<ska::KeyboardInputGUIContext>(ska::EnumContextManager::CONTEXT_GUI);

		gc->requireModule<GraphicModule>("Graphics", gc->getEventDispatcher(), std::move(dc), std::move(renderer), std::move(window));

		return std::make_unique<Game>(core.getEntityManager(), std::move(gc));
	}
}

void LogsConfiguration() {
	//ska::LoggerFactory::staticAccess<ska::WorldCollisionResponse>().configureLogLevel(ska::EnumLogLevel::SKA_DISABLED);
}

Game::Game(ska::EntityManager& em, GameConfPtr&& gc) :
	GameCore(std::forward<GameConfPtr>(gc)),
	m_entityManager(em) {

	LogsConfiguration();

	ska::SDLFont::DEFAULT_FONT_FILE = "./Resources/Fonts/FiraSans-Medium.ttf";
	navigateToState<StateSandbox>(m_entityManager, m_eventDispatcher);
}

int Game::onTerminate(ska::TerminateProcessException& tpe) {
	SLOG(ska::LogLevel::Info) << tpe.what();
	return 0;
}

int Game::onException(ska::GenericException& e) {
	/* Handles Generics Game exceptions */
	std::cerr << e.what() << std::endl;
	return 0;
}

#undef main
int main(int argc, char* argv[]) {
	auto app = ska::GameAppGet();
	app->run();
	return 0;
}
