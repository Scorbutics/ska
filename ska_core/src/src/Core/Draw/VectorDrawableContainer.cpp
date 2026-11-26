#include <algorithm>
#include "VectorDrawableContainer.h"
#include "Renderer.h"


ska::VectorDrawableContainer::VectorDrawableContainer(Renderer& renderer):
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
	// Stable : the drawables of the same priority keep the order they were added in (ex : the layers of a map)
	std::stable_sort(m_data.begin(), m_data.end(), Drawable::staticOperatorInf);
	// The world zoomed (the camera), then the interface over it
	auto inWorld = m_worldZoom != 1.F;
	if (inWorld) {
		m_renderer.beginWorld(m_worldZoom);
	}
	for (auto& d : m_data) {
		if (inWorld && d->getPriority() >= m_interfacePriority) {
			m_renderer.endWorld();
			inWorld = false;
		}
		d->render(m_renderer);
	}
	if (inWorld) {
		m_renderer.endWorld();
	}
	// Set again at each frame
	m_worldZoom = 1.F;
}
