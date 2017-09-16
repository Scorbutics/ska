#pragma once
#include "../ECS/ISystem.h"
#include "CameraAware.h"

namespace ska {
	class DrawableContainer;
	class CameraSystem;

	/**
	 * \brief Base behaviour of any graphic system, meaning that it needs to be linked with a camera system and to have a reference to the current Drawable container.
	 */
	class IGraphicSystem :
	    virtual public ISystem,
	    public CameraAware {
	public:
		IGraphicSystem() = default;
		virtual ~IGraphicSystem() = default;

		virtual void setDrawables(DrawableContainer& container) = 0;
	};
}
