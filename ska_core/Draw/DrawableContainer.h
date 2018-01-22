#pragma once
#include "DrawableFixedPriority.h"
#include "DrawableContainer.h"

namespace ska {
	/**
	 * \brief Container that is used to sort any layerable component of the game, that inherits Drawable.
	 *
	 * This container is provided to every part of the engine that has the power to make things appear on the screen (mainly states and systems).
	 * It also tracks the maximum priority of added elements, in order to allow the user to be able to add an element on "top" of every others.
	 */
	class DrawableContainer {
	public:
        DrawableContainer() :
			m_topPriority2D(std::numeric_limits<int>::min()),
			m_topPriority(std::numeric_limits<int>::min()) {}


		/**
		 * \brief Draws the whole container, which generally implies drawing each element.
		 */
		virtual void draw() = 0;

		/**
		 * \brief Resets the container to make it empty.
		 */
		virtual void clear() = 0;

		virtual ~DrawableContainer() = default;


		/**
		 * \brief Adds a Drawable element to the container
		 * \param d The drawable to add
		 */
		void add(const Drawable& d) {
			if (d.isVisible()) {
				auto currentPriority = d.getPriority();
				auto currentPriority2D = d.getPriority2D();
				if (currentPriority2D > m_topPriority2D) {
					m_topPriority2D = currentPriority2D;
				}

				if (currentPriority > m_topPriority) {
					m_topPriority = currentPriority;
				}

				push(d);
			}
		}

		/**
		 * \brief Adds a Drawable element on the top of every other elements.
		 * \param d The Drawable
		 */
		void addHead(DrawableFixedPriority& d) {
			if (d.isVisible()) {
				d.setPriority(++m_topPriority);
				add(d);
			}
		}

		/**
		* \brief Adds a Drawable element on the top of every other elements, based on 2D priority.
		* \param d The Drawable
		*/
		void addHead2D(DrawableFixedPriority& d) {
			if (d.isVisible()) {
				d.setPriority(++m_topPriority2D);
				add(d);
			}
		}

	protected:
		virtual void push(const Drawable& d) = 0;
    private:
		int m_topPriority2D;
		int m_topPriority;
	};
}

