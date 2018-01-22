#include <algorithm>
#include "VectorDrawableContainer.h"


ska::VectorDrawableContainer::VectorDrawableContainer(const Renderer& renderer):
	m_renderer(renderer) {
}

void ska::VectorDrawableContainer::push(const Drawable& d) {
	m_data.push_back(&d);
}

void ska::VectorDrawableContainer::clear() {
	m_data.clear();
}

void ska::VectorDrawableContainer::reserve(const unsigned int size) {
	m_data.reserve(size);
}

void ska::VectorDrawableContainer::draw() {
	sort(m_data.begin(), m_data.end(), Drawable::staticOperatorInf);
	for (auto& d : m_data) {
		d->render(m_renderer);
	}
}
