#include "PriorityQueueDrawableContainer.h"


ska::PriorityQueueDrawableContainer::PriorityQueueDrawableContainer(Renderer& renderer) : 
	m_renderer(renderer) {
}

void ska::PriorityQueueDrawableContainer::push(const Drawable& d) {
	m_data.push(&d);
}

void ska::PriorityQueueDrawableContainer::clear() {
	while (!m_data.empty()) {
		m_data.pop();
	}
}

void ska::PriorityQueueDrawableContainer::draw() {
	for (const auto d = m_data.top(); !m_data.empty(); m_data.pop()) {
		if (d != nullptr) {
			d->render(m_renderer);
		}
	}
}

ska::PriorityQueueDrawableContainer::~PriorityQueueDrawableContainer() {
}
