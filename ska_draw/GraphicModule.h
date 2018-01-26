#pragma once
#include <memory>
#include "Utils/SubObserver.h"
#include "Data/Events/StateEvent.h"
#include "Data/Events/GameEventDispatcher.h"
#include "Module.h"

namespace ska {
    class DrawableContainer;
    class Renderer;
	class Window;


	class GraphicModule :
		public Module,
		public SubObserver<StateEvent> {

		using RendererPtr = std::unique_ptr<Renderer>;
		using DrawableContainerPtr = std::unique_ptr<DrawableContainer>;
        using WindowPtr = std::unique_ptr<Window>;

	public:
		GraphicModule(const std::string& name, GameEventDispatcher& ged, DrawableContainerPtr dc, RendererPtr renderer, WindowPtr window);
		void graphicUpdate(unsigned int ellapsedTime, StateHolder& sh) override;
		virtual ~GraphicModule();

    private:
        GameEventDispatcher& m_eventDispatcher;
        DrawableContainerPtr m_drawables;
        RendererPtr m_renderer;
        WindowPtr m_mainWindow;
		bool onStateEvent(StateEvent& se);
	};
}
