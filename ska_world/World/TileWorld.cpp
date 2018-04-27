#include <algorithm>
#include "Layer.h"
#include "Utils/StringUtils.h"
#include "Utils/RectangleUtils.h"
#include "LayerEvent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Graphic/System/CameraSystem.h"
#include "Draw/DrawableContainer.h"
#include "TileWorld.h"
#include "TileWorldLoader.h"
#include "Utils/FileUtils.h"
#include "Data/Events/WorldEvent.h"

ska::TileWorld::TileWorld(GameEventDispatcher& ged, Tileset& tileset) :
	m_eventDispatcher(ged),
	m_blocksX(0),
	m_blocksY(0),
	m_blockSize(tileset.getTileSize()),
	m_tileset(&tileset) {
}

ska::TileWorld::TileWorld(GameEventDispatcher& ged, Tileset& tileset, const TileWorldLoader& loader) :
	TileWorld(ged, tileset) {
	load(loader);
}

bool ska::TileWorld::isSameBlockId(const Point<int>& p1, const Point<int>& p2, int layerIndex) const {
	const auto p1Block = p1 / m_blockSize;
	const auto p2Block = p2 / m_blockSize;
	if (p1Block.x >= static_cast<int>(m_blocksX) ||
		p2Block.x >= static_cast<int>(m_blocksX) ||
		p1Block.y >= static_cast<int>(m_blocksY) ||
		p2Block.y >= static_cast<int>(m_blocksY)) {
		return true;
	}

	const auto& b1 = m_physics.getBlock(layerIndex, p1Block.x, p1Block.y);
	const auto& b2 = m_physics.getBlock(layerIndex, p2Block.x, p2Block.y);
	return b1 == b2 || (b1 != nullptr && b2 != nullptr && b1->id == b2->id);
}

bool ska::TileWorld::isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const {
	const auto blockPos = pos / m_blockSize;
	if (blockPos.x >= m_blocksX || blockPos.y >= m_blocksY) {
		return true;
	}
	const auto& b = m_physics.getBlock(0, blockPos.x, blockPos.y);
	const auto result = b != nullptr ? (authorizedBlocks.find(b->id.x + b->id.y * m_tileset->getWidth()) != authorizedBlocks.end()) : false;
	return result;
}

bool ska::TileWorld::getCollision(const unsigned int x, const unsigned int y) const {
	return m_physics.collide(x, y);
}

void ska::TileWorld::update(const ska::Rectangle& cameraPos) {
	for (auto& graphicLayer : m_graphicLayers) {
		graphicLayer->update(cameraPos);
	}
}

void ska::TileWorld::graphicUpdate(unsigned int, ska::DrawableContainer& drawables) {
	for (const auto& graphicLayer : m_graphicLayers) {
		drawables.add(*graphicLayer);
	}
}

const ska::Tile* ska::TileWorld::getHighestBlock(const std::size_t x, const std::size_t y) const {
	const auto layers = m_physics.layers();
	for(auto i = layers - 1; i >= 0; i--) {
		const auto& b = m_physics.getBlock(i, x, y);
		if(b != nullptr && b->collision != ska::TileCollision::Void) {
			return b;
		}
	}
	return nullptr;
}

void ska::TileWorld::load(const TileWorldLoader& loader, Tileset* tilesetToChange) {
	m_autoScripts = true;

	const auto tilesetChanged = tilesetToChange != nullptr && m_tileset->getName() != tilesetToChange->getName();
	if(tilesetChanged) {
		m_tileset = tilesetToChange;
	}

	const auto worldChanged = loader.getName() != m_fullName;
	if (worldChanged || tilesetChanged) {
		m_fullName = loader.getName();

		m_physics = loader.loadPhysics(*m_tileset);
		m_graphicLayers = loader.loadGraphics(*m_tileset, m_blockSize);

		if (m_physics.empty() || m_graphicLayers.empty() || m_physics.layers() != m_graphicLayers.size()) {
			throw IllegalStateException("Map invalide : pas suffisamment de donnees concernant les couches.");
		}

		m_blocksX = m_physics.getLayer(0).getBlocksX();
		m_blocksY = m_physics.getLayer(0).getBlocksY();

		m_events = loader.loadEvents(m_blocksX, m_blocksY);

        const ska::FileNameData fndata(m_fullName);
        m_name = fndata.name;

        auto we = WorldEvent { WorldEventType::WORLD_CHANGE };
        we.blocksWidth = m_blocksX;
        we.blocksHeight = m_blocksX;
        we.blockSize = m_blockSize;
        we.fullName = m_fullName;
        m_eventDispatcher.Observable<WorldEvent>::notifyObservers(we);
	}
}

//TODO déplacer dans les layers event
ska::ScriptSleepComponent* ska::TileWorld::chipsetScriptAuto() {
	if (m_autoScripts) {
		auto tmp = m_tilesetEvent->getScript(ScriptTriggerType::AUTO);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->context = m_fullName;
				m_autoScripts = false;
				return ssc;
			}
		}
	}
	return nullptr;
}

//TODO déplacer dans les layers event
std::vector<ska::ScriptSleepComponent*> ska::TileWorld::chipsetScript(const Point<int>& oldPos, const Point<int>& frontPos, ScriptTriggerType type) {
	std::vector<ScriptSleepComponent*> result;
	if (m_tilesetEvent == nullptr || type == ScriptTriggerType::AUTO) {
		return result;
	}

	const auto effectiveBlockPosition = type == ScriptTriggerType::MOVE_OUT ? oldPos / m_blockSize : frontPos / m_blockSize;

	const int layers = m_physics.layers();
	for (auto i = layers; i >= 0; i--) {
		const auto& b = m_physics.getBlock(i, effectiveBlockPosition.x, effectiveBlockPosition.y);
		if (b != nullptr && b->collision != TileCollision::Void) {

			auto tmp = m_tilesetEvent->getScript(type, b->id);

			const auto newBlock = frontPos / m_blockSize;
			const auto oldBlock = oldPos / m_blockSize;

			for (auto& ssc : tmp) {
				if (ssc != nullptr) {
					ssc->args.clear();
					ssc->args.push_back(StringUtils::intToStr(oldBlock.x));
					ssc->args.push_back(StringUtils::intToStr(oldBlock.y));
					ssc->args.push_back(StringUtils::intToStr(newBlock.x));
					ssc->args.push_back(StringUtils::intToStr(newBlock.y));
					ssc->args.push_back(StringUtils::intToStr(RectangleUtils::getDirectionFromPos(oldBlock * m_blockSize, newBlock * m_blockSize)));
					ssc->context = m_fullName;
					result.push_back(ssc);
				}
			}
		}
	}

	return result;

}

ska::Point<int> ska::TileWorld::alignOnBlock(const Rectangle& hitbox) const {
	const auto hitBoxBlock = (Point<int>(hitbox) / m_blockSize) * m_blockSize;
	return Point<int>(hitbox) - hitBoxBlock;
}

ska::Rectangle ska::TileWorld::placeOnNearestPracticableBlock(const Rectangle& hitBox, const unsigned int radius) const {
	std::vector<Rectangle> blocksPos;
	auto hitBoxBlock = (Point<int>(hitBox) + Point<int>(hitBox.w, hitBox.h) / 2) / m_blockSize;
	auto result = hitBox;

	if (radius == 0) {
		return result;
	}

	const auto maxWidth = getBlocksX();
	const auto maxHeight = getBlocksY();

	Rectangle blockArea{};
	blockArea.x = hitBoxBlock.x - radius;
	blockArea.y = hitBoxBlock.y - radius;
	blockArea.w = (radius << 1) + 1;
	blockArea.h = (radius << 1) + 1;

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
			const Rectangle rect{ static_cast<int>(x + blockArea.x), static_cast<int>(y + blockArea.y), hitBox.w, hitBox.h};
			blocksPos.push_back(rect);
		}
	}

	std::sort(blocksPos.begin(), blocksPos.end(), [hitBoxBlock](decltype(*blocksPos.begin())& it1, decltype(*blocksPos.begin())& it2) -> bool {
		return RectangleUtils::distanceSquared<int>(it1, hitBoxBlock) < RectangleUtils::distanceSquared<int>(it2, hitBoxBlock);
	});

	for (const auto& r : blocksPos) {
		if (!getCollision(r.x, r.y)) {
			result = r;
			result.x *= m_blockSize;
			result.y *= m_blockSize;
			break;
		}
	}

	return result;
}

std::size_t ska::TileWorld::getBlocksX() const{
    return m_blocksX;
}

std::size_t ska::TileWorld::getPixelWidth() const {
	return m_blocksX * m_blockSize;
}

std::size_t ska::TileWorld::getBlocksY() const{
	return m_blocksY;
}

std::size_t ska::TileWorld::getPixelHeight() const {
	return m_blocksY * m_blockSize;
}

std::size_t ska::TileWorld::getBlockSize() const {
	return m_blockSize;
}




