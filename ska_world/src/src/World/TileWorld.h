#pragma once

#include <map>
#include <memory>
#include <unordered_set>
#include <vector>
#include "Core/ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Core/Data/Events/GameEventDispatcher.h"
#include "TilesetEvent.h"
#include "Tileset.h"
#include "CollisionProfile.h"
#include "LayerRenderable.h"
#include "LayerEvent.h"

namespace ska {

	class TileWorldLoader;
	class ScriptSleepComponent;
	class DrawableContainer;

	/**
	 * \brief The map : a grid of chunks, each with its layers, its tileset and its events. A single map (load) is a grid
	 * of one chunk ; a large world (reset) has its chunks added and removed while it is played (addChunk, removeChunk).
	 * The blocks and the pixels are those of the whole world.
	 */
	class TileWorld :
		public MovableNonCopyable,
	    public ScriptPositionedGetter {
	public:
		TileWorld(GameEventDispatcher& ged, Tileset& tileset);
		// A world of chunks, each with its own tileset (reset, addChunk)
		TileWorld(GameEventDispatcher& ged, unsigned int blockSize);
		TileWorld(GameEventDispatcher& ged, Tileset& tileset, const TileWorldLoader& loader);
		TileWorld(TileWorld&&) = default;

		// A single map : the world is its only chunk (loaded again only when its name or its tileset changes)
		void load(const TileWorldLoader& loader, Tileset* tilesetToChange = nullptr);

		// An empty world of chunksX x chunksY chunks of chunkBlocksX x chunkBlocksY blocks, with "layers" layers
		void reset(const std::string& name, unsigned int chunksX, unsigned int chunksY, unsigned int chunkBlocksX, unsigned int chunkBlocksY, unsigned int layers);
		// The map of a chunk (the size of the chunks, else an error), drawn with its tileset, which lives while the chunk
		// is there. autoScripts : its events of the loading of the map run (getScriptsAuto)
		void addChunk(const Point<int>& chunk, const TileWorldLoader& loader, Tileset& tileset, bool autoScripts = true);
		void removeChunk(const Point<int>& chunk);
		bool hasChunk(const Point<int>& chunk) const;
		// The collisions of a chunk (its own blocks), nullptr : it is not there
		const CollisionProfile* chunkCollisions(const Point<int>& chunk) const;
		// The tileset of the chunk of a block, nullptr : it is not there
		Tileset* tilesetAt(int blockX, int blockY) const;

		unsigned int getChunksX() const { return m_chunksX; }
		unsigned int getChunksY() const { return m_chunksY; }
		unsigned int getChunkBlocksX() const { return m_chunkBlocksX; }
		unsigned int getChunkBlocksY() const { return m_chunkBlocksY; }

		std::size_t getPixelWidth() const;
		std::size_t getPixelHeight() const;

		std::size_t getBlocksX() const;
		std::size_t getBlocksY() const;

		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables);
		bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const;

		std::size_t getBlockSize() const override;

		ScriptPack getScripts(const Point<int>& oldCenterPos, const Point<int>& frontPos, ScriptTriggerType type, const Point<int>* lastBlockDirection) override;
		ScriptGlobalPtrPack getScriptsAuto() override;

		const CollisionProfile& getCollisionProfile() const;

	private:
		struct Chunk {
			std::string name;
			CollisionProfile collisions;
			std::vector<LayerRenderablePtr> graphics;
			std::vector<LayerEventPtr> events;
			Tileset* tileset;
			bool autoScripts;
		};

		std::unique_ptr<Chunk> buildChunk(const Point<int>& chunk, const TileWorldLoader& loader, Tileset& tileset, CollisionProfile collisions, bool autoScripts) const;
		void notifyChanged();

	    GameEventDispatcher& m_eventDispatcher;

		unsigned int m_blocksX{};
		unsigned int m_blocksY{};
		unsigned int m_blockSize;
		unsigned int m_chunksX{};
		unsigned int m_chunksY{};
		unsigned int m_chunkBlocksX{};
		unsigned int m_chunkBlocksY{};

		std::string m_fullName;
		std::string m_name;

		// By (x, y) : the same order of drawing and of events every time
		std::map<std::pair<int, int>, std::unique_ptr<Chunk>> m_chunks;

		// The tileset of a single map (load)
		Tileset* m_tileset = nullptr;
		CollisionProfile m_collisionProfile;
	};
}
