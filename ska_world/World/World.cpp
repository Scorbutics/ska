#include <algorithm>
#include "World.h"
#include "Layer.h"
#include "Block.h"
#include "Utils/StringUtils.h"
#include "Utils/RectangleUtils.h"
#include "LayerEvent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Graphic/System/CameraSystem.h"
#include "Exceptions/FileException.h"
#include "Utils/FileUtils.h"
#include "ECS/Basics/Physic/CollisionContact.h"

ska::World::World(const unsigned int tailleBloc) :
    m_windDirection(0),
    m_nbrBlockX(0),
    m_nbrBlockY(0),
    m_blockSize(tailleBloc),
	m_autoScriptsPlayed(false),
	m_cameraSystem(nullptr),
    m_lBot(*this),
	m_lMid(*this, &m_lBot),
	m_lTop(*this, &m_lMid),
	m_layerE(*this),
	m_chipset("." FILE_SEPARATOR "Chipsets" FILE_SEPARATOR "corr.png") {
}

void ska::World::linkCamera(CameraSystem* cs) {
	m_cameraSystem = cs;
	if (m_cameraSystem != nullptr) {
		m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
	}
}

unsigned int ska::World::getNumberLayers() const {
	return 3;
}

bool ska::World::isSameBlockId(const Point<int>& p1, const Point<int>& p2, int layerIndex) const {
	const Layer* l;
	switch (layerIndex) {
	case 0:
		l = &m_lBot;
		break;
	case 1:
		l = &m_lMid;
		break;
	case 2:
		l = &m_lTop;
		break;
	default :
		l = nullptr;
		break;
	}

	const auto p1Block = p1 / m_blockSize;
	const auto p2Block = p2 / m_blockSize;
	if (p1Block.x >= m_nbrBlockX || p2Block.x >= m_nbrBlockX || p1Block.y >= m_nbrBlockY || p2Block.y >= m_nbrBlockY) {
		return true;
	}

	Block* b1 = l == nullptr ? getHigherBlock(p1Block.x, p1Block.y) : l->getBlock(p1Block.x, p1Block.y);
	Block* b2 = l == nullptr ? getHigherBlock(p2Block.x, p2Block.y) : l->getBlock(p2Block.x, p2Block.y);
	return (b1 == b2 || (b1 != nullptr && b2 != nullptr && b1->getID() == b2->getID()));
}

bool ska::World::isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const {
	auto l = &m_lBot;
	const auto blockPos = pos / m_blockSize;
	if (blockPos.x >= m_nbrBlockX || blockPos.y >= m_nbrBlockY ) {
		return true;
	}
	auto b = l->getBlock(blockPos.x, blockPos.y);
	const auto result = b != nullptr ? (authorizedBlocks.find(b->getID()) != authorizedBlocks.end()) : false;
	return result;
}

ska::ChipsetHolder& ska::World::getChipset() {
	return m_chipset;
}

const std::string& ska::World::getFileName() const {
	return m_fileName;
}

bool ska::World::isBlockDodgeable(const int i, const int j) const {
	Block* b = m_lMid.getBlock(i, j);
	return (b != nullptr && b->getProperties() == BLOCK_PROP_JUMPWALL);
}

bool ska::World::getCollision(const int i, const int j) const {
	if (m_lBot.getBlockCollision(i, j) == BLOCK_COL_YES &&
		(m_lMid.getBlockCollision(i, j) != BLOCK_COL_NO)) {
		return true;
	}

	if (m_lMid.getBlockCollision(i, j) == BLOCK_COL_YES) {
		return true;
	}

    return false;
}

void ska::World::update() {
	m_lBot.getRenderable().update();
	m_lMid.getRenderable().update();
	m_lTop.getRenderable().update();
}

bool ska::World::intersectBlocksAtPos(const Rectangle& hitbox, std::vector<Point<int>>& outputX, std::vector<Point<int>>& outputY) const {
	Point<int> horizontalSegment { hitbox.x, hitbox.x + hitbox.w };
	Point<int> verticalSegment { hitbox.y, hitbox.y + hitbox.h };
	horizontalSegment /= m_blockSize;
	verticalSegment /= m_blockSize;

	auto col = false;

	for (auto x = horizontalSegment.x; x <= horizontalSegment.y; x++) {
		for (auto y = verticalSegment.x; y <= verticalSegment.y; y++) {
			if (getCollision(x, y)) {
				const Rectangle hitboxBlock{ static_cast<int>(x * m_blockSize), static_cast<int>(y * m_blockSize), static_cast<int>(m_blockSize), static_cast<int>(m_blockSize) };

				//Vertical
				const auto collisionContact = CollisionContact(hitbox, hitboxBlock);
				if(collisionContact.normal().y != 0) {
					outputY.push_back(hitboxBlock);
					col = true;
				}

				//Horizontal
				if (collisionContact.normal().x != 0) {
					outputX.push_back(hitboxBlock);
					col = true;
				}
			}
		}
	}
	return col;
}

const ska::Rectangle* ska::World::getView() const {
	return m_cameraSystem == nullptr ? nullptr : m_cameraSystem->getDisplay();
}

ska::LayerRenderable& ska::World::getLayerRenderable(int level) {
	switch (level) {
	case 0:
		return m_lBot.getRenderable();
	case 1:
		return m_lMid.getRenderable();
	default:
		return m_lTop.getRenderable();
	}
}

ska::LayerEvent& ska::World::getLayerEvent() {
	return m_layerE;
}

std::string ska::World::getName() const {
    return m_worldName;
}

std::string ska::World::getGenericName() const {
    return m_genericName;
}

void ska::World::load(const std::string& fileName, const std::string& chipsetName) {
	const auto worldChanged = fileName != m_fileName;
	m_autoScriptsPlayed = false;
	const auto chipsetChanged = m_chipset.attach(m_blockSize, chipsetName);

	if (worldChanged) {
		const ska::FileNameData fndata(fileName);
		m_genericName = fndata.name;
		m_worldName = fndata.path + "/" + fndata.name;
		m_fileName = fileName;

		getData();
	}

	if (worldChanged || chipsetChanged) {
		const auto fileNamePrefix = m_worldName + std::string(FILE_SEPARATOR) + m_genericName;
		const auto& botLayerName = fileNamePrefix + ".bmp";
		const auto& midLayerName = fileNamePrefix + "M.bmp";
		const auto& topLayerName = fileNamePrefix + "T.bmp";
		const auto& eventLayerName = fileNamePrefix + "E.txt";

		m_lBot.clear();
		m_lMid.clear();
		m_lTop.clear();

		m_lBot.reset(botLayerName, chipsetName);
		m_lMid.reset(midLayerName, chipsetName);
		m_lTop.reset(topLayerName, chipsetName);

		m_layerE.changeLevel(eventLayerName);

		if (m_cameraSystem != nullptr) {
			m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
		}
	}

}

std::vector<ska::ScriptSleepComponent*> ska::World::chipsetScript(const Point<int>& oldPos, const Point<int>& newPos, const Point<int>& posToLookAt, const ScriptTriggerType& reason, const unsigned int layerIndex) {
	std::vector<ScriptSleepComponent*> result;
	Layer* l;
	switch (layerIndex) {
	case 0:
		l = &m_lBot;
		break;
	case 1:
		l = &m_lMid;
		break;
	default:
		l = &m_lTop;
		break;
	}

	if (reason == EnumScriptTriggerType::AUTO) {
		auto tmp = m_chipset.getScript("", reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->context = getName();
				result.push_back(ssc);
			}
		}
		return result;
	}

	const auto newBlock = newPos / m_blockSize;
	const auto oldBlock = oldPos / m_blockSize;
	auto b = l->getBlock(posToLookAt.x / m_blockSize, posToLookAt.y / m_blockSize);
	if (b != nullptr) {
		const auto id = b->getID();
		auto tmp = m_chipset.getScript(StringUtils::intToStr(id), reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->args.clear();
				ssc->args.push_back(StringUtils::intToStr(oldBlock.x));
				ssc->args.push_back(StringUtils::intToStr(oldBlock.y));
				ssc->args.push_back(StringUtils::intToStr(newBlock.x));
				ssc->args.push_back(StringUtils::intToStr(newBlock.y));
				ssc->args.push_back(StringUtils::intToStr(RectangleUtils::getDirectionFromPos(oldBlock * m_blockSize, newBlock * m_blockSize)));
				ssc->context = getName();
				result.push_back(ssc);
			}
		}
	}
	return result;

}

ska::Point<int> ska::World::alignOnBlock(const Rectangle& hitbox) const {
	const auto hitBoxBlock = (Point<int>(hitbox) / m_blockSize) * m_blockSize;
	return Point<int>(hitbox) - hitBoxBlock;
}

ska::Rectangle ska::World::placeOnNearestPracticableBlock(const Rectangle& hitBox, const unsigned int radius) const {
	std::vector<Rectangle> blocksPos;
	auto hitBoxBlock = (Point<int>(hitBox) + Point<int>(hitBox.w, hitBox.h) / 2) / m_blockSize;
	auto result = hitBox;

	if (radius == 0) {
		return result;
	}

	const auto maxWidth = getNbrBlocX();
	const auto maxHeight = getNbrBlocY();

	Rectangle blockArea;
	blockArea.x = hitBoxBlock.x - radius;
	blockArea.y = hitBoxBlock.y - radius;
	blockArea.w = (radius << 1) + 1;
	blockArea.h = (radius << 1) + 1;

	if (blockArea.x < 0) {
		blockArea.x = 0;
	}

	if (blockArea.y < 0) {
		blockArea.y = 0;
	}

	if (blockArea.x + 1 > static_cast<int>(maxWidth)) {
		blockArea.x = maxWidth - 1;
	}

	if (blockArea.y + 1 > static_cast<int>(maxHeight)) {
		blockArea.y = maxHeight - 1;
	}

	if (blockArea.x + blockArea.w > static_cast<int>(maxWidth)) {
		blockArea.w = maxWidth - blockArea.x;
	}

	if (blockArea.y + blockArea.h > static_cast<int>(maxHeight)) {
		blockArea.h = maxHeight - blockArea.y;
	}

	for (unsigned int x = 0; x != static_cast<unsigned int>(blockArea.w); x++) {
		for (unsigned int y = 0; y != static_cast<unsigned int>(blockArea.h); y++) {
			const Rectangle rect{ static_cast<int>(x + blockArea.x), static_cast<int>(y + blockArea.y), hitBox.w, hitBox.h};
			blocksPos.push_back(rect);
		}
	}

	std::sort(blocksPos.begin(), blocksPos.end(), [hitBoxBlock](decltype(*blocksPos.begin())& it1, decltype(*blocksPos.begin())& it2) -> bool {
		return RectangleUtils::distanceSquared<int>(it1, hitBoxBlock) < RectangleUtils::distanceSquared<int>(it2, hitBoxBlock);
	});

	for (const auto& r : blocksPos) {
		if (!getCollision(r.x, r.y)) {
			result = r;
			result.x *= m_blockSize;
			result.y *= m_blockSize;
			break;
		}
	}

	return result;
}

ska::Block* ska::World::getHigherBlock(const unsigned int i, const unsigned int j) const {
	Block* bBot = m_lBot.getBlock(i, j);
	Block* bMid = m_lMid.getBlock(i, j);
	Block* bTop = m_lTop.getBlock(i, j);

	if (bTop != nullptr) {
		return bTop;
	}

	if (bMid != nullptr) {
		return bMid;
	}

	return bBot;
}


unsigned ska::World::getNbrBlocX() const{
    return m_nbrBlockX;
}

unsigned int ska::World::getPixelWidth() const {
	return m_nbrBlockX*m_blockSize;
}

unsigned ska::World::getNbrBlocY() const{
	return m_nbrBlockY;
}

unsigned int ska::World::getPixelHeight() const {
	return m_nbrBlockY*m_blockSize;
}

unsigned int ska::World::getBlockSize() const {
	return m_blockSize;
}

void ska::World::setNbrBlocX(unsigned int nbrBlockX) {
	m_nbrBlockX = nbrBlockX;
}

void ska::World::setNbrBlocY(unsigned int nbrBlockY) {
	m_nbrBlockY = nbrBlockY;
}



void ska::World::getRainFromData(std::string ){
    /*int idsprite, acceleration, density;
	IniReader reader(stringDataFile);

    if(reader.exists("Rain id_sprite")) {
		idsprite = reader.get<int>("Rain id_sprite");
		acceleration = reader.get<int>("Rain acceleration");
		density = reader.get<int>("Rain density");
    } else {
        std::clog << "La pluie est inexistante sur cette map" << std::endl;
    }*/
}

void ska::World::getMobSettingsFromData() {
	m_mobSettings.clear();

	unsigned int i = 0;
	try {
        do  {
            m_mobSettings.push_back(IniReader( "." FILE_SEPARATOR "Levels" FILE_SEPARATOR "" + m_genericName + "" FILE_SEPARATOR "Monsters" FILE_SEPARATOR "" + StringUtils::intToStr(i) + ".ini"));
            i++;
        } while(m_mobSettings[i-1].isLoaded());
    } catch (FileException&) {
        SKA_LOG_INFO("Number of monsters found on map : ", i);
    }

	//le dernier �l�ment est invalide, on le supprime donc
	if (!m_mobSettings.empty()) {
		m_mobSettings.pop_back();
	}

}

void ska::World::getData() {
	const auto stringDataFile = "." FILE_SEPARATOR "Levels" FILE_SEPARATOR "" + m_genericName + "" FILE_SEPARATOR "" + m_genericName + ".ini";

    getRainFromData(stringDataFile);
	getMobSettingsFromData();
}

std::vector<ska::IniReader>& ska::World::getMobSettings() {
	return m_mobSettings;
}

