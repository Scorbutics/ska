#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "GraphicModule.h"
#include "Exceptions/IllegalStateException.h"
#include "Exceptions/ExceptionTrigger.h"
#include "Logging/Logger.h"
#include "Draw/Renderer.h"
#include "Core/State/StateHolder.h"
#include "Draw/DrawableContainer.h"
#include "Core/Window.h"
#include "SDLTTFLibrary.h"
#include "SDLImageLibrary.h"
#include "SDLLibrary.h"

ska::GraphicModule::GraphicModule(const std::string& moduleName, GameEventDispatcher& ged, DrawableContainerPtr dc, RendererPtr renderer, WindowPtr window):
	SubObserver<StateEvent>(std::bind(&GraphicModule::onStateEvent, this, std::placeholders::_1), ged),
	m_eventDispatcher(ged),
	m_drawables(std::move(dc)),
	m_renderer(std::move(renderer)),
	m_mainWindow(std::move(window)) {

	SDLLibrary::get().setMainReady();

	if (SDLLibrary::get().init(SDL_INIT_VIDEO) < 0) {
		ExceptionTrigger<ska::IllegalStateException>("Erreur lors de l'initialisation de la SDL : " + std::string(SDLLibrary::get().getError()));
	}

	/* Fix GDB Bug with named thread on windows (Mixer raises an exception when init) */
	if (!SDLLibrary::get().setHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1")) {
		SLOG(LogLevel::Warn) << "Attention : Windows nomme actuellement les threads créés par l'application alors que le programme tente de désactiver cette fonctionnalité.";
	}

	if (!SDLLibrary::get().setHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
		SLOG(LogLevel::Warn) << "Attention : Linear texture filtering impossible à activer.";
	}

	if (!(SDLImageLibrary::get().init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		ExceptionTrigger<ska::IllegalStateException>("Erreur lors de l'initialisation de SDL_image : " + std::string(SDLLibrary::get().getError()));
	}

	if (SDLTTFLibrary::get().init() == -1) {
		SLOG(LogLevel::Error) << "Erreur d'initialisation de TTF_Init : " << SDLLibrary::get().getError();
	}

	m_mainWindow->load();
	m_renderer->load();
	m_renderer->setDefaultColor({ 127, 127, 127, 255 });

	m_mainWindow->show();
}

bool ska::GraphicModule::onStateEvent(StateEvent& se) {
    if(se.type == StateEventType::FIRST_STATE_LOAD) {
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
	SDLTTFLibrary::get().quit();
	SDLImageLibrary::get().quit();
	SDLLibrary::get().quit();
}
