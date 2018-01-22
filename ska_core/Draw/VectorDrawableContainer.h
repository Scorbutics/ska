#pragma once
#include <vector>
#include <functional>

#include "DrawableContainerBase.h"

namespace ska {

	/**
	 * \brief A vector based implementation of a container of Drawable
	 * This implementation will have to trigger a sort before every draw in order to respect priorities.
	 */
	class VectorDrawableContainer :
		public DrawableContainerBase<std::vector<const Drawable*>> {

	public:
		VectorDrawableContainer(const Renderer& renderer);

        virtual void reserve(const unsigned int size);
		virtual void clear() override;
		virtual void draw() override;
		virtual ~VectorDrawableContainer() = default;

	protected:
		virtual void push(const Drawable& d) override;

	private:
		const Renderer& m_renderer;
	};
}
