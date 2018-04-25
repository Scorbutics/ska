#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "GraphicModule.h"
#include "Exceptions/IllegalStateException.h"
#include "Logging/Logger.h"
#include "Draw/Renderer.h"
#include "Core/State/StateHolder.h"
#include "Draw/DrawableContainer.h"
#include "Core/Window.h"

ska::GraphicModule::GraphicModule(const std::string& moduleName, GameEventDispatcher& ged, DrawableContainerPtr dc, RendererPtr renderer, WindowPtr window):
	SubObserver<StateEvent>(std::bind(&GraphicModule::onStateEvent, this, std::placeholders::_1), ged),
	m_eventDispatcher(ged),
	m_drawables(std::move(dc)),
	m_renderer(std::move(renderer)),
	m_mainWindow(std::move(window)) {

	SDL_SetMainReady();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw ska::IllegalStateException("Erreur lors de l'initialisation de la SDL : " + std::string(SDL_GetError()));
	}

	/* Fix GDB Bug with named thread on windows (Mixer raises an exception when init) */
	if (!SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1")) {
		SKA_LOG_MESSAGE("Attention : Windows nomme actuellement les threads créés par l'application alors que le programme tente de désactiver cette fonctionnalité.");
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
		SKA_LOG_MESSAGE("Attention : Linear texture filtering impossible à activer.");
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		throw ska::IllegalStateException("Erreur lors de l'initialisation de SDL_image : " + std::string(IMG_GetError()));
	}

	if (TTF_Init() == -1) {
		SKA_LOG_ERROR("Erreur d'initialisation de TTF_Init : ", TTF_GetError());
	}

	m_mainWindow->load();
	m_renderer->load();

	m_mainWindow->show();
}

bool ska::GraphicModule::onStateEvent(StateEvent& se) {
    if(se.type == FIRST_STATE_LOADED) {
        GameEvent ge(GameEventType::GAME_WINDOW_READY);
        ge.windowWidth = m_mainWindow->getWidth();
        ge.windowHeight = m_mainWindow->getHeight();
        m_eventDispatcher.ska::Observable<GameEvent>::notifyObservers(ge);
        return true;
    }
    return false;
}

void ska::GraphicModule::graphicUpdate(unsigned int ellapsedTime, StateHolder& sh) {
    sh.graphicUpdate(ellapsedTime, *m_drawables);
    m_drawables->draw();

    m_drawables->clear();
    m_renderer->update();
}

ska::GraphicModule::~GraphicModule() {
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
