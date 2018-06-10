#include "Exceptions/ExceptionTrigger.h"
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
#include "LayerEventLoaderTilesetEvent.h"

ska::TileWorld::TileWorld(GameEventDispatcher& ged, Tileset& tileset) :
	m_eventDispatcher(ged),
	m_blocksX(0),
	m_blocksY(0),
	m_blockSize(tileset.getTileSize()),
	m_tileset(&tileset),
	m_collisionProfile(tileset.getTileSize()) {
}

ska::TileWorld::TileWorld(GameEventDispatcher& ged, Tileset& tileset, const TileWorldLoader& loader) :
	TileWorld(ged, tileset) {
	load(loader);
}

bool ska::TileWorld::isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const {
	const auto blockPos = pos / m_blockSize;
	const auto& b = m_collisionProfile.getBlock(0, blockPos.x, blockPos.y);
	const auto result = b != nullptr ? (authorizedBlocks.find(b->id.x + b->id.y * m_tileset->getWidth()) != authorizedBlocks.end()) : false;
	return result;
}

void ska::TileWorld::graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) {
	for (const auto& graphicLayer : m_graphicLayers) {
		graphicLayer->graphicUpdate(cameraPos, drawables);
	}
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

		m_collisionProfile = loader.loadPhysics(*m_tileset);
		m_graphicLayers = loader.loadGraphics(*m_tileset, m_blockSize);

		if (m_collisionProfile.empty() || m_graphicLayers.empty() || m_collisionProfile.layers() != m_graphicLayers.size()) {
			ExceptionTrigger<IllegalStateException>("Map invalide : pas suffisamment de donnees concernant les couches.");
		}

		m_blocksX = m_collisionProfile.getBlocksX();
		m_blocksY = m_collisionProfile.getBlocksY();

		//Layer Events
		m_events = loader.loadEvents(m_blocksX, m_blocksY);

		//Tileset based layer events
		const auto layers = m_collisionProfile.layers();
		for (auto i = 0u; i < layers; i++) {
			m_events.push_back(std::make_unique<LayerEvent>(LayerEventLoaderTilesetEvent{ m_collisionProfile, i, m_tileset->getTilesetEvent() }, m_blocksX, m_blocksY));
		}

        const ska::FileNameData fndata(m_fullName);
        m_name = fndata.name;

        auto we = ska::WorldEvent { WorldEventType::WORLD_CHANGE };
        we.blocksWidth = m_blocksX;
        we.blocksHeight = m_blocksX;
        we.blockSize = m_blockSize;
        we.fullName = m_fullName;
        m_eventDispatcher.Observable<WorldEvent>::notifyObservers(we);
	}
}

std::vector<std::reference_wrapper<ska::ScriptSleepComponent>> ska::TileWorld::getScriptsAuto() {
	std::vector<std::reference_wrapper<ScriptSleepComponent>> result;
	if (m_autoScripts) {
		for (auto& layerScriptsPtr : m_events) {
			auto& autosScript = layerScriptsPtr->getAutoScript();
			for (auto& ssc : autosScript) {
				ssc.context = m_fullName;
				m_autoScripts = false;
				result.emplace_back(ssc);
			}
		}
	}
	return {};
}

std::vector<std::reference_wrapper<ska::ScriptSleepComponent>> ska::TileWorld::getScripts(const Point<int>& oldCenterPos, const Point<int>& frontPos, ScriptTriggerType type, const Point<float>* normal) {
	std::vector<std::reference_wrapper<ScriptSleepComponent>> result;
	if (type == ScriptTriggerType::AUTO) {
		return result;
	}

	const auto effectiveBlockPosition = type == ScriptTriggerType::MOVE_OUT ? oldCenterPos / m_blockSize : frontPos / m_blockSize;

	for (auto& layerScriptsPtr : m_events) {
		if (effectiveBlockPosition.x >= m_blocksX ||
			effectiveBlockPosition.y >= m_blocksX) {
			continue;
		}
		auto& blockScript = layerScriptsPtr->getScripts(effectiveBlockPosition);

		const auto newBlock = frontPos / m_blockSize;
		const auto oldBlock = oldCenterPos / m_blockSize;

		for (auto& ssc : blockScript) {
			if (type == ssc.triggeringType) {
				ssc.args.clear();
				ssc.args.push_back(StringUtils::intToStr(oldBlock.x));
				ssc.args.push_back(StringUtils::intToStr(oldBlock.y));
				ssc.args.push_back(StringUtils::intToStr(newBlock.x));
				ssc.args.push_back(StringUtils::intToStr(newBlock.y));
				if (normal != nullptr) {
					ssc.args.push_back(StringUtils::intToStr(RectangleUtils::getDirectionFromPos(newBlock * m_blockSize, ska::Point<int>(newBlock.x - normal->x, newBlock.y - normal->y) * m_blockSize)));
				} else {
					ssc.args.push_back(StringUtils::intToStr(RectangleUtils::getDirectionFromPos(oldBlock * m_blockSize, newBlock * m_blockSize)));
				}
				ssc.context = m_fullName;
				result.emplace_back(ssc);
			}
		}
		
	}

	return result;

}

const ska::CollisionProfile& ska::TileWorld::getCollisionProfile() const {
	return m_collisionProfile;
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




