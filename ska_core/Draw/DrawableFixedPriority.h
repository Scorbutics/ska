#pragma once
#include "Drawable.h"
#include "../Core/Priorized.h"

namespace ska {
	
	/**
	 * \brief A concrete Drawable that has a priority fixed during his whole lifetime.
	 */
	class DrawableFixedPriority :
        virtual public Drawable,
        public Priorized {
	public:
		DrawableFixedPriority() = default;

		int getPriority() const override {
            return Priorized::getPriority();
		}

		virtual ~DrawableFixedPriority() = default;

	};
}
