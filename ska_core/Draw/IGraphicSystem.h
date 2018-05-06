#pragma once
#include "../ECS/ISystem.h"

namespace ska {
	class DrawableContainer;

	/**
	 * \brief Base behaviour of any graphic system, meaning that it needs to have a reference to the current Drawable container.
	 */
	class IGraphicSystem :
	    virtual public ISystem {
	public:
		IGraphicSystem() = default;
		virtual ~IGraphicSystem() = default;

		virtual void setDrawables(DrawableContainer& container) = 0;
	};
}
