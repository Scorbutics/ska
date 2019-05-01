#pragma once
#include <limits>
#include "DrawableContainer.h"

namespace ska {
	/**
	 * \brief Container that is used to sort any layerable component of the game, that inherits Drawable.
	 *
	 * This container is provided to every part of the engine that has the power to make things appear on the screen (mainly states and systems).
	 * It also tracks the maximum priority of added elements, in order to allow the user to be able to add an element on "top" of every others.
	 */
    template <class Container>
	class DrawableContainerBase :
        public DrawableContainer {
	public:
        DrawableContainerBase() = default;

        auto begin() {
            return m_data.begin();
        }

        auto end() {
            return m_data.end();
        }

        auto cbegin() const{
            return m_data.cbegin();
        }

        auto cend() const {
            return m_data.cend();
        }

		virtual ~DrawableContainerBase() = default;

    protected:
        Container m_data;
	};
}

