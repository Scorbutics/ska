#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "Graphic/__internalConfig/LoggerConfig.h"
#include "GraphicModule.h"
#include "Core/Exceptions/IllegalStateException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Logging/Logger.h"
#include "Core/Draw/Renderer.h"
#include "Core/State/StateHolder.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Window.h"
#include "Graphic/SDLTTFLibrary.h"
#include "Graphic/SDLImageLibrary.h"
#include "Core/SDLLibrary.h"

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
	// SDL quits at the end of the program (SDLLibrary) : the other modules (ex : the sound) are destroyed after this one
	SDLTTFLibrary::get().quit();
}
