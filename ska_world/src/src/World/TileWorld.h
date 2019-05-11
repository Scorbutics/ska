#pragma once

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

	class TileWorld :
		public MovableNonCopyable,
	    public ScriptPositionedGetter {
	public:
		TileWorld(GameEventDispatcher& ged, Tileset& tileset);
		TileWorld(GameEventDispatcher& ged, Tileset& tileset, const TileWorldLoader& loader);
		TileWorld(TileWorld&&) = default;

		void load(const TileWorldLoader& loader, Tileset* tilesetToChange = nullptr);

		std::size_t getPixelWidth() const;
		std::size_t getPixelHeight() const;

		std::size_t getBlocksX() const;
		std::size_t getBlocksY() const;

		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables);
		bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const;

		std::size_t getBlockSize() const;

		ScriptPack getScripts(const Point<int>& oldCenterPos, const Point<int>& frontPos, ScriptTriggerType type, const Point<int>* lastBlockDirection) override;
		ScriptGlobalPtrPack getScriptsAuto() override;

		const CollisionProfile& getCollisionProfile() const;

	private:
		bool m_autoScripts {};
	    GameEventDispatcher& m_eventDispatcher;

		unsigned int m_blocksX;
		unsigned int m_blocksY;
		unsigned int m_blockSize;

		std::string m_fullName;
		std::string m_name;

		std::vector<LayerRenderablePtr> m_graphicLayers{};
		std::vector<LayerEventPtr> m_events;
		
		gsl::not_null<Tileset*> m_tileset;
		CollisionProfile m_collisionProfile;
	};
}
