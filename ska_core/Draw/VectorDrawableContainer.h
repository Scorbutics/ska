#pragma once
#include <vector>
#include <functional>

#include "DrawableContainer.h"

namespace ska {
	
	/**
	 * \brief A vector based implementation of a container of Drawable
	 * This implementation will have to trigger a sort before every draw in order to respect priorities.
	 */
	class VectorDrawableContainer :
		public DrawableContainer {
		using VectorDrawable = std::vector<const Drawable*>;

	public:
		VectorDrawableContainer(const Renderer& renderer);
		virtual void reserve(const unsigned int size);
		VectorDrawable::iterator begin();
		VectorDrawable::iterator end();
		VectorDrawable::const_iterator cbegin() const;
		VectorDrawable::const_iterator cend() const;


		virtual void clear() override;
		virtual void draw() override;
		virtual ~VectorDrawableContainer();
	
	protected:
		virtual void push(const Drawable& d) override;

	private:
		VectorDrawable m_data;
		const Renderer& m_renderer;
	};
}
