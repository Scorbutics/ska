#pragma once

#include <unordered_set>
#include <vector>
#include "Inputs/Readers/IniReader.h"
#include "Draw/CameraAware.h"
#include "ChipsetHolder.h"
#include "LayerEvent.h"
#include "ECS/Basics/Physic/BlockAllowance.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Data/Events/GameEventDispatcher.h"
#include "LayerRenderable.h"
#include "TilesetEvent.h"
#include "Tileset.h"
#include "CollisionProfile.h"

namespace ska {
	class TileWorldLoader;
	class CameraSystem;
	class PhysicObject;
	struct Tile;
	class PrefabEntityManager;
	class ScriptSleepComponent;
	class DrawableContainer;

	class TileWorld :
		public MovableNonCopyable,
	    public BlockAllowance,
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

		Point<int> getBlockId(const Point<int>& pos, int layerIndex) const override;
		virtual void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables);
		bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const override;
		bool getCollision(unsigned int x, unsigned int y) const override;

		unsigned int getBlockSize() const override;

		std::vector<std::reference_wrapper<ScriptSleepComponent>> getScripts(const Point<int>& oldCenterPos, const Point<int>& frontPos, ScriptTriggerType type) override;
		std::vector<std::reference_wrapper<ScriptSleepComponent>> getScriptsAuto() override;

		Rectangle placeOnNearestPracticableBlock(const Rectangle& hitBox, const unsigned int radius) const;
		Point<int> alignOnBlock(const Rectangle& hitbox) const;

		const Tile* getHighestBlock(std::size_t x, std::size_t y) const;

	private:
		bool m_autoScripts {};
	    GameEventDispatcher& m_eventDispatcher;

		unsigned int m_blocksX;
		unsigned int m_blocksY;
		unsigned int m_blockSize;

		std::vector<IniReader> m_mobSettings{};
		std::string m_fullName;
		std::string m_name;

		std::vector<LayerRenderablePtr> m_graphicLayers{};
		std::vector<LayerEventPtr> m_events;
		CollisionProfile m_physicLayers;

		gsl::not_null<Tileset*> m_tileset;
	};
}
