#pragma once
#include <queue>
#include "DrawableContainerBase.h"

namespace ska {
    class Drawable;
	class PriorityQueueDrawableContainer :
		public DrawableContainerBase<std::priority_queue<const Drawable*>> {
	public:
		PriorityQueueDrawableContainer(Renderer& renderer);

		virtual void clear() override;
		virtual void draw() override;
		virtual ~PriorityQueueDrawableContainer() = default;

	protected:
		virtual void push(const Drawable& d) override;
	private:
		/* FIXME : operator< sur Drawable ne marchera pas ici : On utilise des pointeurs.
			Il faut par exemple utiliser unique_ptr et surcharger l'operator< de comparaison entre
			unique_ptr */
		Renderer& m_renderer;
	};
}
