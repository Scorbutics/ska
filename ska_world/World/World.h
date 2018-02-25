#pragma once

#include <unordered_set>
#include <vector>
#include "Inputs/Readers/IniReader.h"
#include "Graphic/Texture.h"
#include "Draw/CameraAware.h"
#include "ChipsetHolder.h"
#include "Layer.h"
#include "LayerEvent.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "Data/BlockContainer.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
#include "ChipsetEvent.h"
#include "Chipset.h"

namespace ska {
	class CameraSystem;
	class PhysicObject;
	class Block;
	class PrefabEntityManager;
	class ScriptSleepComponent;
	class DrawableContainer;
	typedef char ScriptTriggerType;

	class World :
	    public CameraAware,
	    public BlockContainer,
	    public CollisionProfile,
	    public ScriptPositionedGetter {
	public:
		World(const unsigned int tailleBloc, const std::string& chipsetCorrespondanceFilename);
		
		World(const World&) = delete;
		World& operator=(const World&) = delete;

		virtual void load(const std::string& fileName, const std::string& chipsetName);

		std::vector<IniReader>& getMobSettings();
		std::string getGenericName() const;
		std::string getName() const;
		const std::string& getFileName() const;

		unsigned int getPixelWidth() const;
		unsigned int getPixelHeight() const;

		void update();

		unsigned int getNbrBlocX() const;
		unsigned int getNbrBlocY() const;

		LayerRenderable& getLayerRenderable(int level);
		LayerEvent& getLayerEvent();
		unsigned int getNumberLayers() const;

		const Rectangle* getView() const;
		Chipset* getChipset();

		Block* getHigherBlock(const unsigned int i, const unsigned int j) const;

		void getData();
		bool isSameBlockId(const Point<int>& p1, const Point<int>& p2, int layerIndex) const override;
		bool intersectBlocksAtPos(const Rectangle& hitbox, std::vector<Rectangle>& outputX, std::vector<Rectangle>& outputY) const override;
		virtual void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables);
		bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const override;

		bool getCollision(const int i, const int j) const;
		bool isBlockDodgeable(const int i, const int j) const;

		unsigned int getBlockSize() const;

		/* TODO classe � part ? */
		std::vector<ScriptSleepComponent*> chipsetScript(const Point<int>& oldPos, const Point<int>& newPos, const Point<int>& p, const ScriptTriggerType& reason, unsigned int layerIndex) override;
		Rectangle placeOnNearestPracticableBlock(const Rectangle& hitBox, const unsigned int radius) const;
		Point<int> alignOnBlock(const Rectangle& hitbox) const;

		void linkCamera(CameraSystem* cs) override;

		virtual ~World() = default;

	private:
		static void getRainFromData(std::string stringDataFile);
		void getMobSettingsFromData();

		int m_windDirection;
		int m_nbrBlockX, m_nbrBlockY;
		unsigned int m_blockSize;


		std::string m_fileName, m_genericName, m_worldName;

		bool m_autoScriptsPlayed;
		std::vector<IniReader> m_mobSettings;
		CameraSystem* m_cameraSystem;

	protected:
		Layer m_lBot;
		Layer m_lMid;
		Layer m_lTop;
		LayerEvent m_layerE;
		
		const ChipsetCorrespondanceMapper m_correspondanceMapper;
		std::unique_ptr<Chipset> m_chipset;
		std::unique_ptr<ChipsetEvent> m_chipsetEvent;
	};
}
