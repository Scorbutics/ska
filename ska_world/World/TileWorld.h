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
#include "CollisionProfile.h"
#include "LayerRenderable.h"
#include "TilesetEvent.h"
#include "Tileset.h"

namespace ska {
	class TileWorldLoader;
	class CameraSystem;
	class PhysicObject;
	struct Tile;
	class PrefabEntityManager;
	class ScriptSleepComponent;
	class DrawableContainer;
	using ScriptTriggerType = char;

	class TileWorld :
		public MovableNonCopyable,
	    public CameraAware,
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

		const Rectangle* getView() const;

		bool isSameBlockId(const Point<int>& p1, const Point<int>& p2, int layerIndex) const override;
		virtual void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables);
		bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const override;

		bool getCollision(std::size_t x, std::size_t y) const override;

		unsigned int getBlockSize() const override;

		/* TODO classe à part ? */
		std::vector<ScriptSleepComponent*> chipsetScript(const Point<int>& oldPos, const Point<int>& newPos, const Point<int>& p, const ScriptTriggerType& reason, unsigned int layerIndex) override;
		Rectangle placeOnNearestPracticableBlock(const Rectangle& hitBox, const unsigned int radius) const;
		Point<int> alignOnBlock(const Rectangle& hitbox) const;

		void linkCamera(CameraSystem* cs) override;

		const Tile* getHighestBlock(std::size_t x, std::size_t y) const;

	private:
	    GameEventDispatcher& m_eventDispatcher;

		unsigned int m_blocksX;
		unsigned int m_blocksY;
		unsigned int m_blockSize;

		bool m_autoScriptsPlayed;
		std::vector<IniReader> m_mobSettings{};
		CameraSystem* m_cameraSystem;
		std::string m_fullName;
		std::string m_name;

		std::vector<LayerRenderablePtr> m_graphicLayers{};
		std::vector<LayerEventPtr> m_events;
		CollisionProfile m_collisionProfile;

		//TODO Tileset avec lifetime à part
		gsl::not_null<Tileset*> m_tileset;
		std::unique_ptr<TilesetEvent> m_tilesetEvent{};
	};
}
