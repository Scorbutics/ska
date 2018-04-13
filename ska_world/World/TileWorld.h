#pragma once

#include <unordered_set>
#include <vector>
#include "Inputs/Readers/IniReader.h"
#include "Draw/CameraAware.h"
#include "ChipsetHolder.h"
#include "LayerEvent.h"
#include "ECS/Basics/Physic/BlockAllowance.h"
#include "Data/BlockContainer.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
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
	    public BlockContainer,
	    public BlockAllowance,
	    public ScriptPositionedGetter {
	public:
		TileWorld(Tileset& tileset);
		TileWorld(Tileset& tileset, const TileWorldLoader& loader);
		TileWorld(TileWorld&&) = default;

		void load(const TileWorldLoader& loader, Tileset* tilesetToChange = nullptr);

		unsigned int getPixelWidth() const;
		unsigned int getPixelHeight() const;

		void update(const ska::Rectangle& cameraPos);

		unsigned int getNbrBlocX() const;
		unsigned int getNbrBlocY() const;

		const Rectangle* getView() const;

		bool isSameBlockId(const Point<int>& p1, const Point<int>& p2, int layerIndex) const override;
		bool intersectBlocksAtPos(const Rectangle& hitbox, std::vector<Rectangle>& outputX, std::vector<Rectangle>& outputY) const override;
		virtual void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables);
		bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const override;

		bool getCollision(unsigned int x, unsigned int y) const;

		unsigned int getBlockSize() const override;

		/* TODO classe à part ? */
		std::vector<ScriptSleepComponent*> chipsetScript(const Point<int>& oldPos, const Point<int>& newPos, const Point<int>& p, const ScriptTriggerType& reason, unsigned int layerIndex) override;
		Rectangle placeOnNearestPracticableBlock(const Rectangle& hitBox, const unsigned int radius) const;
		Point<int> alignOnBlock(const Rectangle& hitbox) const;

		void linkCamera(CameraSystem* cs) override;

		const Tile* getHighestBlock(unsigned i, unsigned y) const;

	private:

		int m_nbrBlockX, m_nbrBlockY;
		unsigned int m_blockSize;


		std::string m_fileName, m_genericName, m_worldName;

		bool m_autoScriptsPlayed;
		std::vector<IniReader> m_mobSettings{};
		CameraSystem* m_cameraSystem;

	protected:
		std::vector<LayerRenderablePtr> m_graphicLayers{};
		CollisionProfile m_collisionProfile;
		LayerEvent m_layerE;
		
		//TODO Tileset avec lifetime à part
		gsl::not_null<Tileset*> m_tileset;
		std::unique_ptr<TilesetEvent> m_tilesetEvent{};		
	};
}
