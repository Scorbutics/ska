#pragma once
#include "Drawable.h"
#include "../Priorized.h"

namespace ska {
	
	/**
	 * \brief A concrete Drawable that has a priority fixed during his whole lifetime.
	 */
	class DrawableFixedPriority :
        public Drawable {
	public:
		DrawableFixedPriority() = default;

		int getPriority() const override {
            return Priorized::getPriority();
		}

		virtual ~DrawableFixedPriority() = default;

	};
}
