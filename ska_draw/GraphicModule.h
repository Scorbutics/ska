#pragma once
#include <memory>
#include "Module.h"

namespace ska {
    class DrawableContainer;
    class Renderer;

	class GraphicModule :
		public ska::Module {
		using RendererPtr = std::unique_ptr<Renderer>;
		using DrawableContainerPtr = std::unique_ptr<DrawableContainer>;

	public:
		GraphicModule(const std::string& name, DrawableContainerPtr dc, RendererPtr renderer);
		void graphicUpdate(unsigned int ellapsedTime, StateHolder& sh) override;
		virtual ~GraphicModule();

    private:
        DrawableContainerPtr m_drawables;
        RendererPtr m_renderer;
	};
}
