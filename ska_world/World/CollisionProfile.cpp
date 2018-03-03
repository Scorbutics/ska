#include <optional>

#include "Exceptions/IllegalStateException.h"
#include "CollisionProfile.h"
#include <cassert>

void ska::CollisionProfile::calculate() const {
	unsigned int width;
	unsigned int height;
	std::tie(width, height) = safeGetSizes();

	for(auto x = 0; x < width; x++) {
		for(auto y = 0; y < height; y++) {
			m_collisions[x][y] = getHigherCollision(x, y);
		}
	}
}

ska::Layer& ska::CollisionProfile::addLayer(LayerPtr l) {
	assert(l != nullptr);
	auto& result = *l.get();
	m_layers.push_back(std::move(l));
	m_mustUpdateCollisions = true;
	return result;
}

ska::Layer& ska::CollisionProfile::getLayer(const unsigned int index) {
	return *m_layers[index].get();
}

bool ska::CollisionProfile::collide(const unsigned int x, const unsigned int y) const {
	if (m_mustUpdateCollisions) {
		calculate();
		m_mustUpdateCollisions = false;
	}
	return m_collisions[x][y] == BlockCollision::YES;
}

ska::Block const* ska::CollisionProfile::getBlock(const unsigned int layer, const unsigned x, const unsigned y) const {
	const auto& l = m_layers[layer];
	return l->getBlock(x, y);
}

void ska::CollisionProfile::clear() {
	m_layers.clear();
	m_collisions.clear();
	m_mustUpdateCollisions = true;
}

std::pair<unsigned, unsigned> ska::CollisionProfile::safeGetSizes() const {
	std::optional<unsigned int> width;
	std::optional<unsigned int> height;
	
	for(const auto& l : m_layers) {
		if (!width.has_value() || !height.has_value()) {
			width = l->getBlocksX();
			height = l->getBlocksY();
			m_collisions.resize(l->getBlocksX(), l->getBlocksY());
		} else if(width != l->getBlocksX() || height != l->getBlocksY()) {
			//TODO message
			throw IllegalStateException("");
		}
	}

	return std::make_pair(width.value_or(0), height.value_or(0));
}

ska::BlockCollision ska::CollisionProfile::getHigherCollision(const unsigned int x, const unsigned int y) const {
	auto nonVoidCollision = BlockCollision::VOID;

	for (auto it = m_layers.crbegin(); it != m_layers.crend(); ++it) {
		auto& l = *it;
		
		const auto& collision = l->getCollision(x, y);
		if (collision == BlockCollision::NO) {
			return collision;
		}
		
		if(collision != BlockCollision::VOID) {
			nonVoidCollision = collision;
		}
	}
	return nonVoidCollision;
}
