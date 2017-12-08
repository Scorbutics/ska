#pragma once
#include "Renderable.h"

namespace ska {
	class Renderer;

	/**
	 * \brief Forces classes to implement a way to be rendered in a certain order (by priority)
	 */
	class Drawable {
	public:
		Drawable() = default;
		virtual ~Drawable() = default;
		
		virtual void render(const Renderer& renderer) const = 0;

		static bool staticOperatorInf(const Drawable* a, const Drawable* b) {
			const auto p1 = a->getPriority();
			const auto p2 = b->getPriority();
			return p1 < p2;
		}

		/**
		 * \brief Defines an inferior operator comparison
		 * \param id The other Drawable to compare with
		 * \return true if the current Drawable has less priority than the parameter, false otherwise
		 */
		bool operator< (const Drawable &id) const {
			const auto p1 = getPriority();
			const auto p2 = id.getPriority();
			return p1 < p2;
		}
		
		static bool staticOperatorSup(const Drawable* a, const Drawable* b) {
			const auto p1 = a->getPriority();
			const auto p2 = b->getPriority();
			return p1 > p2;
		}

		/**
		* \brief Defines an superior operator comparison
		* \param id The other Drawable to compare with
		* \return true if the current Drawable has more priority than the parameter, false otherwise
		*/
		bool operator> (const Drawable &id) const {
			const auto p1 = getPriority();
			const auto p2 = id.getPriority();
			return p1 > p2;
		}

		/**
		* \brief Gets the 2D priority of the current Drawable.
		* \return The 2D priority of this Drawable
		* 
		* The 2D priority is defined when z-ordering is taken as a variable into priority.
		* The 2D priority ignores this z-ordering.
		*/
		virtual int getPriority2D() const {
			return getPriority();
		}


		/**
		* \brief Gets the priority of the current Drawable.
		* \return The priority of this Drawable
		*/
		virtual int getPriority() const = 0;


		/**
		 * \brief Visibility accessor
		 * \return true if the current Drawable is visible, false otherwise
		 */
		virtual bool isVisible() const = 0;
	};
}
