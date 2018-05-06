#pragma once

#include <unordered_set>
#include <vector>
#include "LayerEvent.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Data/Events/GameEventDispatcher.h"
#include "LayerRenderable.h"
#include "TilesetEvent.h"
#include "Tileset.h"
#include "CollisionProfile.h"

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

		void update(const ska::Rectangle& cameraPos);

		std::size_t getBlocksX() const;
		std::size_t getBlocksY() const;

		virtual void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables);
		bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const;

		unsigned int getBlockSize() const;

		std::vector<std::reference_wrapper<ScriptSleepComponent>> getScripts(const Point<int>& oldCenterPos, const Point<int>& frontPos, ScriptTriggerType type) override;
		std::vector<std::reference_wrapper<ScriptSleepComponent>> getScriptsAuto() override;

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
		CollisionProfile m_physicLayers;
	};
}
