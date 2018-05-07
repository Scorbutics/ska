#include <cassert>
#include <optional>

#include "Exceptions/IllegalStateException.h"
#include "CollisionProfile.h"
#include "Utils/StringUtils.h"
#include "Utils/RectangleUtils.h"

void ska::CollisionProfile::calculate() {
	std::tie(m_blocksX, m_blocksY) = safeGetSizes();
	
	if (m_blocksX != 0 && m_blocksY != 0) {
		m_collisions.resize(m_layers.size());
		auto index = 0u;
		for (auto& col : m_collisions) {
			col.resize(m_blocksX, m_blocksY);
			for (auto x = 0u; x < m_blocksX; x++) {
				for (auto y = 0u; y < m_blocksY; y++) {
					const auto collision = getHighestCollidingBlock(index, x, y);
					col[x][y] = collision == nullptr ? TileCollision::No : TileCollision::Yes;
				}
			}
			index++;
		}
	}
}

ska::CollisionProfile::CollisionProfile(const unsigned int blockSize) :
	CollisionProfile(blockSize, std::vector<LayerPtr>{}) {
}

ska::CollisionProfile::CollisionProfile(const unsigned int blockSize, std::vector<LayerPtr> layers) :
	m_blockSize(blockSize) {
	
	for (auto& l : layers) {
		assert(l != nullptr);
		m_layers.emplace_back(std::move(l));
	}

	calculate();
}

bool ska::CollisionProfile::collide(const std::size_t layerMax, const std::size_t blockX, const std::size_t blockY) const {
	if(layerMax >= m_collisions.size()) {
		return true;
	}
	return !m_collisions[layerMax].has(blockX, blockY) || m_collisions[layerMax][blockX][blockY] == TileCollision::Yes;
}

const ska::Tile* ska::CollisionProfile::getBlock(const std::size_t layer, const std::size_t blockX, const std::size_t blockY) const {
	return 	m_layers[layer]->getBlock(blockX, blockY);
}

bool ska::CollisionProfile::empty() const {
	return layers() == 0;
}

std::size_t ska::CollisionProfile::layers() const {
	return m_layers.size();
}

ska::Rectangle ska::CollisionProfile::placeOnNearestPracticableBlock(const std::size_t layerMax, const Rectangle& hitBox, unsigned int radius) const {
	std::vector<Rectangle> blocksPos;
	auto hitBoxBlock = (Point<int>(hitBox) + Point<int>(hitBox.w, hitBox.h) / 2) / m_blockSize;
	auto result = hitBox;

	if (radius == 0) {
		return result;
	}

	const auto maxWidth = getBlocksX();
	const auto maxHeight = getBlocksY();

	auto blockArea = Rectangle{
		static_cast<int>(hitBoxBlock.x - radius) ,
		static_cast<int>(hitBoxBlock.y - radius),
		static_cast<int>((radius << 1) + 1),
		static_cast<int>((radius << 1) + 1)
	};

	if (blockArea.x < 0) {
		blockArea.x = 0;
	}

	if (blockArea.y < 0) {
		blockArea.y = 0;
	}

	if (blockArea.x + 1 > static_cast<int>(maxWidth)) {
		blockArea.x = maxWidth - 1;
	}

	if (blockArea.y + 1 > static_cast<int>(maxHeight)) {
		blockArea.y = maxHeight - 1;
	}

	if (blockArea.x + blockArea.w > static_cast<int>(maxWidth)) {
		blockArea.w = maxWidth - blockArea.x;
	}

	if (blockArea.y + blockArea.h > static_cast<int>(maxHeight)) {
		blockArea.h = maxHeight - blockArea.y;
	}

	for (unsigned int x = 0; x != static_cast<unsigned int>(blockArea.w); x++) {
		for (unsigned int y = 0; y != static_cast<unsigned int>(blockArea.h); y++) {
			const Rectangle rect{ static_cast<int>(x + blockArea.x), static_cast<int>(y + blockArea.y), hitBox.w, hitBox.h };
			blocksPos.push_back(rect);
		}
	}

	std::sort(blocksPos.begin(), blocksPos.end(), [hitBoxBlock](decltype(*blocksPos.begin())& it1, decltype(*blocksPos.begin())& it2) -> bool {
		return RectangleUtils::distanceSquared<int>(it1, hitBoxBlock) < RectangleUtils::distanceSquared<int>(it2, hitBoxBlock);
	});

	for (const auto& r : blocksPos) {
		if (!collide(layerMax, r.x, r.y)) {
			result = r;
			result.x *= m_blockSize;
			result.y *= m_blockSize;
			break;
		}
	}

	return result;
	
}

ska::Point<int> ska::CollisionProfile::alignOnBlock(const Rectangle& hitbox) const {
	const auto hitBoxBlock = (Point<int>(hitbox) / m_blockSize) * m_blockSize;
	return Point<int>(hitbox) - hitBoxBlock;
}

unsigned int ska::CollisionProfile::getBlockSize() const {
	return m_blockSize;
}

std::size_t ska::CollisionProfile::getBlocksX() const {
	return m_blocksX;
}

std::size_t ska::CollisionProfile::getBlocksY() const {
	return m_blocksY;
}

const ska::Tile* ska::CollisionProfile::getHighestCollidingBlock(const std::size_t layerTop, const std::size_t blockX, const std::size_t blockY) const {
	const Tile* voidCollidingTile = nullptr;
	if(layerTop >= m_layers.size()) {
		return nullptr;
	}

	for (auto it = m_layers.crbegin() + layerTop; it != m_layers.crend(); ++it) {
		auto& l = *it->get();

		const auto& collision = l.getCollision(blockX, blockY);
		const auto& block = l.getBlock(blockX, blockY);
		switch (collision) {
		case TileCollision::Yes:
			return block;

		//We consider that if the last layer (= bottom) block is a void one and the upper blocks are not colliding, then the void block is a colliding block.
		case TileCollision::Void:
			voidCollidingTile = block;
			break;

		case TileCollision::No:
		default:
			voidCollidingTile = nullptr;
			break;
		}
	}
	return voidCollidingTile;
}

const ska::Tile* ska::CollisionProfile::getHighestNonCollidingBlock(const std::size_t layerTop, const std::size_t blockX, const std::size_t blockY) const {
	if (layerTop >= m_layers.size()) {
		return nullptr;
	}

	for (auto it = m_layers.crbegin() + layerTop; it != m_layers.crend(); ++it) {
		auto& l = *it->get();

		const auto& collision = l.getCollision(blockX, blockY);
		if(collision == TileCollision::No) {		
			return l.getBlock(blockX, blockY);
		}
	}
	return nullptr;
}

std::pair<unsigned, unsigned> ska::CollisionProfile::safeGetSizes() const {
	std::optional<unsigned int> width;
	std::optional<unsigned int> height;

	for(const auto& lRef : m_layers) {
		const auto& l = *lRef;
		if (!width.has_value() || !height.has_value()) {
			width = l.getBlocksX();
			height = l.getBlocksY();
		} else if(width != l.getBlocksX() || height != l.getBlocksY()) {
			throw IllegalStateException("Not every layer has same dimensions meaning that this map is invalid (expected " + 
				StringUtils::uintToStr(width.value()) + " width and " + StringUtils::uintToStr(height.value()) + " height for every layer).");
		}
	}

	return std::make_pair(width.value_or(0), height.value_or(0));
}
