#include "World/__internalConfig/LoggerConfig.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Utils/RectangleUtils.h"
#include "LayerEvent.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Graphic/System/CameraSystem.h"
#include "Core/Draw/DrawableContainer.h"
#include "TileWorld.h"
#include "TileWorldLoader.h"
#include "Base/IO/Files/FileUtils.h"
#include "Core/Data/Events/WorldEvent.h"
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

ska::ScriptGlobalPtrPack ska::TileWorld::getScriptsAuto() {
	auto result = ScriptGlobalPtrPack{};
	if (m_autoScripts) {
		for (auto& layerScriptsPtr : m_events) {
			auto& autosScript = layerScriptsPtr->getAutoScript();
			for (auto& scriptData : autosScript) {
				scriptData.scripts.context = m_fullName;
				m_autoScripts = false;
				result.emplace_back(scriptData);
			}
		}
	}
	return result;
}

std::vector<ska::ScriptSleepComponent> ska::TileWorld::getScripts(const Point<int>& oldCenterPos, const Point<int>& frontPos, ScriptTriggerType type, const Point<int>* lastBlockDirection) {
	std::vector<ScriptSleepComponent> result;
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
				result.emplace_back(ssc);
				auto& addedSsc = result.back();
				addedSsc.args.push_back(StringUtils::intToStr(oldBlock.x));
				addedSsc.args.push_back(StringUtils::intToStr(oldBlock.y));
				addedSsc.args.push_back(StringUtils::intToStr(newBlock.x));
				addedSsc.args.push_back(StringUtils::intToStr(newBlock.y));
				if (lastBlockDirection != nullptr) {
					const auto oldBlockCalculated = ska::Point<int>(newBlock.x + lastBlockDirection->x, newBlock.y + lastBlockDirection->y);
					addedSsc.args.push_back(StringUtils::intToStr(RectangleUtils::getDirectionFromPos(oldBlockCalculated * m_blockSize, newBlock * m_blockSize)));
				} else {
					addedSsc.args.push_back(StringUtils::intToStr(RectangleUtils::getDirectionFromPos(oldBlock * m_blockSize, newBlock * m_blockSize)));
				}
				addedSsc.context = m_fullName;
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




