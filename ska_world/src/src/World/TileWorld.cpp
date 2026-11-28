#include "World/__internalConfig/LoggerConfig.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Core/Exceptions/IllegalStateException.h"
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
	m_blockSize(tileset.getTileSize()),
	m_tileset(&tileset),
	m_collisionProfile(tileset.getTileSize()) {
}

ska::TileWorld::TileWorld(GameEventDispatcher& ged, const unsigned int blockSize) :
	m_eventDispatcher(ged),
	m_blockSize(blockSize),
	m_collisionProfile(blockSize) {
}

ska::TileWorld::TileWorld(GameEventDispatcher& ged, Tileset& tileset, const TileWorldLoader& loader) :
	TileWorld(ged, tileset) {
	load(loader);
}

bool ska::TileWorld::isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const {
	const auto blockPos = pos / m_blockSize;
	const auto& b = m_collisionProfile.getBlock(0, blockPos.x, blockPos.y);
	const auto* tileset = tilesetAt(blockPos.x, blockPos.y);
	const auto result = b != nullptr && tileset != nullptr ? (authorizedBlocks.find(b->id.x + b->id.y * tileset->getWidth()) != authorizedBlocks.end()) : false;
	return result;
}

void ska::TileWorld::graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) {
	const auto width = static_cast<int>(m_chunkBlocksX * m_blockSize);
	const auto height = static_cast<int>(m_chunkBlocksY * m_blockSize);
	for (const auto& [coordinates, chunk] : m_chunks) {
		// The chunks out of the screen are not drawn
		const auto x = coordinates.first * width;
		const auto y = coordinates.second * height;
		if (x > cameraPos.x + cameraPos.w || y > cameraPos.y + cameraPos.h || x + width < cameraPos.x || y + height < cameraPos.y) {
			continue;
		}
		for (const auto& graphicLayer : chunk->graphics) {
			graphicLayer->graphicUpdate(cameraPos, drawables);
		}
	}
}

std::unique_ptr<ska::TileWorld::Chunk> ska::TileWorld::buildChunk(const Point<int>& chunk, const TileWorldLoader& loader, Tileset& tileset, CollisionProfile collisions, const bool autoScripts) const {
	const auto blocksX = static_cast<unsigned int>(collisions.getBlocksX());
	const auto blocksY = static_cast<unsigned int>(collisions.getBlocksY());
	const auto origin = Point<int>{ chunk.x * static_cast<int>(blocksX * m_blockSize), chunk.y * static_cast<int>(blocksY * m_blockSize) };
	auto graphics = loader.loadGraphics(tileset, m_blockSize, origin);

	if (collisions.empty() || graphics.empty() || collisions.layers() != graphics.size()) {
		ExceptionTrigger<IllegalStateException>("Map invalide : pas suffisamment de donnees concernant les couches.");
	}

	//Layer Events
	auto events = loader.loadEvents(blocksX, blocksY);

	auto result = std::make_unique<Chunk>(Chunk{ loader.getName(), std::move(collisions), std::move(graphics), std::move(events), &tileset, autoScripts });
	//Tileset based layer events
	const auto layers = result->collisions.layers();
	for (auto i = 0u; i < layers; i++) {
		result->events.push_back(std::make_unique<LayerEvent>(LayerEventLoaderTilesetEvent{ result->collisions, i, tileset.getTilesetEvent() }, blocksX, blocksY));
	}
	return result;
}

void ska::TileWorld::notifyChanged() {
	const ska::FileNameData fndata(m_fullName);
	m_name = fndata.name;

	auto we = ska::WorldEvent { WorldEventType::WORLD_CHANGE };
	we.blocksWidth = m_blocksX;
	we.blocksHeight = m_blocksY;
	we.blockSize = m_blockSize;
	we.fullName = m_fullName;
	m_eventDispatcher.Observable<WorldEvent>::notifyObservers(we);
}

void ska::TileWorld::load(const TileWorldLoader& loader, Tileset* tilesetToChange) {
	const auto tilesetChanged = tilesetToChange != nullptr && (m_tileset == nullptr || m_tileset->getName() != tilesetToChange->getName());
	if(tilesetChanged) {
		m_tileset = tilesetToChange;
	}

	if (m_tileset == nullptr) {
		ExceptionTrigger<IllegalStateException>("No tileset to load the map " + loader.getName() + " with");
	}
	const auto worldChanged = loader.getName() != m_fullName;
	if (worldChanged || tilesetChanged) {
		auto collisions = loader.loadPhysics(*m_tileset);
		const auto blocksX = static_cast<unsigned int>(collisions.getBlocksX());
		const auto blocksY = static_cast<unsigned int>(collisions.getBlocksY());
		const auto layers = static_cast<unsigned int>(collisions.layers());
		auto chunk = buildChunk({ 0, 0 }, loader, *m_tileset, std::move(collisions), true);

		m_chunks.clear();
		m_fullName = loader.getName();
		m_chunksX = m_chunksY = 1;
		m_chunkBlocksX = m_blocksX = blocksX;
		m_chunkBlocksY = m_blocksY = blocksY;
		m_collisionProfile = CollisionProfile{ m_blockSize, 1, 1, blocksX, blocksY, layers };
		m_collisionProfile.setChunk(0, 0, &chunk->collisions);
		m_chunks.emplace(std::make_pair(0, 0), std::move(chunk));
		notifyChanged();
	} else {
		// The same map again : its loading events run again
		for (auto& [coordinates, chunk] : m_chunks) {
			chunk->autoScripts = true;
		}
	}
}

void ska::TileWorld::reset(const std::string& name, const unsigned int chunksX, const unsigned int chunksY, const unsigned int chunkBlocksX, const unsigned int chunkBlocksY, const unsigned int layers) {
	m_chunks.clear();
	m_fullName = name;
	m_chunksX = chunksX;
	m_chunksY = chunksY;
	m_chunkBlocksX = chunkBlocksX;
	m_chunkBlocksY = chunkBlocksY;
	m_blocksX = chunksX * chunkBlocksX;
	m_blocksY = chunksY * chunkBlocksY;
	m_collisionProfile = CollisionProfile{ m_blockSize, chunksX, chunksY, chunkBlocksX, chunkBlocksY, layers };
	notifyChanged();
}

void ska::TileWorld::addChunk(const Point<int>& chunk, const TileWorldLoader& loader, Tileset& tileset, const bool autoScripts) {
	if (chunk.x < 0 || chunk.y < 0 || chunk.x >= static_cast<int>(m_chunksX) || chunk.y >= static_cast<int>(m_chunksY)) {
		ExceptionTrigger<IllegalStateException>("Chunk out of the world : " + loader.getName());
	}
	removeChunk(chunk);
	auto collisions = loader.loadPhysics(tileset);
	if (collisions.getBlocksX() != m_chunkBlocksX || collisions.getBlocksY() != m_chunkBlocksY) {
		ExceptionTrigger<IllegalStateException>("Chunk " + loader.getName() + " : not the size of the chunks of its world");
	}
	auto built = buildChunk(chunk, loader, tileset, std::move(collisions), autoScripts);
	m_collisionProfile.setChunk(chunk.x, chunk.y, &built->collisions);
	m_chunks.emplace(std::make_pair(chunk.x, chunk.y), std::move(built));
}

void ska::TileWorld::removeChunk(const Point<int>& chunk) {
	const auto found = m_chunks.find({ chunk.x, chunk.y });
	if (found != m_chunks.end()) {
		m_collisionProfile.setChunk(chunk.x, chunk.y, nullptr);
		m_chunks.erase(found);
	}
}

bool ska::TileWorld::hasChunk(const Point<int>& chunk) const {
	return m_chunks.count({ chunk.x, chunk.y }) != 0;
}

const ska::CollisionProfile* ska::TileWorld::chunkCollisions(const Point<int>& chunk) const {
	const auto found = m_chunks.find({ chunk.x, chunk.y });
	return found == m_chunks.end() ? nullptr : &found->second->collisions;
}

ska::Tileset* ska::TileWorld::tilesetAt(const int blockX, const int blockY) const {
	if (blockX < 0 || blockY < 0 || m_chunkBlocksX == 0 || m_chunkBlocksY == 0) {
		return nullptr;
	}
	const auto found = m_chunks.find({ blockX / static_cast<int>(m_chunkBlocksX), blockY / static_cast<int>(m_chunkBlocksY) });
	return found == m_chunks.end() ? nullptr : found->second->tileset;
}

ska::ScriptGlobalPtrPack ska::TileWorld::getScriptsAuto() {
	auto result = ScriptGlobalPtrPack{};
	for (auto& [coordinates, chunk] : m_chunks) {
		if (!chunk->autoScripts) {
			continue;
		}
		for (auto& layerScriptsPtr : chunk->events) {
			auto& autosScript = layerScriptsPtr->getAutoScript();
			for (auto& scriptData : autosScript) {
				scriptData.scripts.context = chunk->name;
				result.emplace_back(scriptData);
			}
		}
		chunk->autoScripts = false;
	}
	return result;
}

std::vector<ska::ScriptSleepComponent> ska::TileWorld::getScripts(const Point<int>& oldCenterPos, const Point<int>& frontPos, ScriptTriggerType type, const Point<int>* lastBlockDirection) {
	std::vector<ScriptSleepComponent> result;
	if (type == ScriptTriggerType::AUTO) {
		return result;
	}

	const auto effectiveBlockPosition = type == ScriptTriggerType::MOVE_OUT ? oldCenterPos / m_blockSize : frontPos / m_blockSize;
	if (effectiveBlockPosition.x < 0 || effectiveBlockPosition.y < 0 ||
		effectiveBlockPosition.x >= static_cast<int>(m_blocksX) || effectiveBlockPosition.y >= static_cast<int>(m_blocksY)) {
		return result;
	}
	const auto found = m_chunks.find({ effectiveBlockPosition.x / static_cast<int>(m_chunkBlocksX), effectiveBlockPosition.y / static_cast<int>(m_chunkBlocksY) });
	if (found == m_chunks.end()) {
		return result;
	}
	const auto local = Point<int>{ effectiveBlockPosition.x % static_cast<int>(m_chunkBlocksX), effectiveBlockPosition.y % static_cast<int>(m_chunkBlocksY) };

	for (auto& layerScriptsPtr : found->second->events) {
		auto& blockScript = layerScriptsPtr->getScripts(local);

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
				addedSsc.context = found->second->name;
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




