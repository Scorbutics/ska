#pragma once
#include <memory>
#include "Utils/SubObserver.h"
//#include "SDLLibrary.h"
#include "Data/Events/StateEvent.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	class StateHolder;
	class DrawableContainer;
    class Renderer;
	class Window;

	class GraphicModule :
		public SubObserver<StateEvent> {

		using RendererPtr = std::unique_ptr<Renderer>;
		using DrawableContainerPtr = std::unique_ptr<DrawableContainer>;
        	using WindowPtr = std::unique_ptr<Window>;

	public:
		GraphicModule(const std::string& name, GameEventDispatcher& ged, DrawableContainerPtr dc, RendererPtr renderer, WindowPtr window);
		void graphicUpdate(unsigned int ellapsedTime, StateHolder& sh);
		virtual ~GraphicModule();

    private:
        GameEventDispatcher& m_eventDispatcher;
	/*SDLTTFLibrary m_ttf;
	SDLImageLibrary m_image;*/
	DrawableContainerPtr m_drawables;
        RendererPtr m_renderer;
        WindowPtr m_mainWindow;
		bool onStateEvent(StateEvent& se);
	};
}
