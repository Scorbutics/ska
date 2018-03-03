#include <algorithm>
#include "World.h"
#include "Layer.h"
#include "Utils/StringUtils.h"
#include "Utils/RectangleUtils.h"
#include "LayerEvent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Graphic/System/CameraSystem.h"
#include "Exceptions/FileException.h"
#include "Utils/FileUtils.h"
#include "ECS/Basics/Physic/CollisionContact.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "Draw/DrawableContainer.h"
#include "LayerLoader.h"

ska::World::World(const unsigned int tailleBloc, const std::string& chipsetCorrespondanceFilename) :
	m_windDirection(0),
	m_nbrBlockX(0),
	m_nbrBlockY(0),
	m_blockSize(tailleBloc),
	m_autoScriptsPlayed(false),
	m_cameraSystem(nullptr),
	m_correspondanceMapper(chipsetCorrespondanceFilename) {
}

void ska::World::linkCamera(CameraSystem* cs) {
	m_cameraSystem = cs;
	if (m_cameraSystem != nullptr) {
		m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
	}
}

bool ska::World::isSameBlockId(const Point<int>& p1, const Point<int>& p2, int layerIndex) const {
	const Layer* l;
	const auto p1Block = p1 / m_blockSize;
	const auto p2Block = p2 / m_blockSize;
	if (p1Block.x >= m_nbrBlockX || p2Block.x >= m_nbrBlockX || p1Block.y >= m_nbrBlockY || p2Block.y >= m_nbrBlockY) {
		return true;
	}

	const auto& b1 = m_collisionProfile.getBlock(layerIndex, p1Block.x, p1Block.y);
	const auto& b2 = m_collisionProfile.getBlock(layerIndex, p2Block.x, p2Block.y);
	return (b1 == b2 || (b1 != nullptr && b2 != nullptr && b1->getID() == b2->getID()));
}

bool ska::World::isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& authorizedBlocks) const {
	const auto blockPos = pos / m_blockSize;
	if (blockPos.x >= m_nbrBlockX || blockPos.y >= m_nbrBlockY ) {
		return true;
	}
	const auto& b = m_collisionProfile.getBlock(0, blockPos.x, blockPos.y);
	const auto result = b != nullptr ? (authorizedBlocks.find(b->getID()) != authorizedBlocks.end()) : false;
	return result;
}

bool ska::World::getCollision(const unsigned int x, const unsigned int y) const {
	return m_collisionProfile.collide(x, y);
}

void ska::World::update(const ska::Rectangle& cameraPos) {
	for (auto& graphicLayer : m_graphicLayers) {
		graphicLayer->update(cameraPos);
	}
}

/*
template <class T>
void EraseAll(std::vector<T>& altered, std::vector<T>& toErase){
	using std::begin;
	using std::end;
	using std::remove_if;
	using std::find;

	altered.erase(remove_if(begin(altered), end(altered),
		[&](auto x) {return find(begin(toErase), end(toErase), x) != end(toErase); }), end(altered));
}


void FindAndEraseDoublons(std::vector<ska::Rectangle>& outputX, std::vector<ska::Rectangle>& outputY) {
	std::vector<ska::Rectangle> doublons;
	if(outputX.size() > outputY.size()) {
		EraseAll(outputY, outputX);
	} else {
		EraseAll(outputX, outputY);
	}

}
*/

//TODO => partie physique
bool ska::World::intersectBlocksAtPos(const Rectangle& hitbox, std::vector<Rectangle>& outputX, std::vector<Rectangle>& outputY) const {
	/*Point<int> horizontalSegment { hitbox.x, hitbox.x + hitbox.w };
	Point<int> verticalSegment { hitbox.y, hitbox.y + hitbox.h };
	horizontalSegment /= m_blockSize;
	verticalSegment /= m_blockSize;

	auto col = false;

	std::vector<ska::Rectangle> intermediateX;
	std::vector<ska::Rectangle> intermediateY;

	for (auto x = horizontalSegment.x; x <= horizontalSegment.y; x++) {
		for (auto y = verticalSegment.x; y <= verticalSegment.y; y++) {
			if (getCollision(x, y)) {
				const Rectangle hitboxBlock{ static_cast<int>(x * m_blockSize), static_cast<int>(y * m_blockSize), static_cast<int>(m_blockSize), static_cast<int>(m_blockSize) };

				//Vertical
				const auto collisionContact = CollisionContact(hitbox, hitboxBlock);
				if (collisionContact.normal().y != 0) {
					SKA_LOG_INFO("Normal Y, overlap ", collisionContact.overlap().x, " ; ", collisionContact.overlap().y, " ; ", collisionContact.overlap().w, " ; ", collisionContact.overlap().h);
					outputY.push_back(hitboxBlock);
					col = true;
				}

				//Horizontal
				if (collisionContact.normal().x != 0) {
					SKA_LOG_INFO("Normal X, overlap ", collisionContact.overlap().x, " ; ", collisionContact.overlap().y, " ; ", collisionContact.overlap().w, " ; ", collisionContact.overlap().h);
					outputX.push_back(hitboxBlock);
					col = true;
				}
			}
		}
	}
	
	FindAndEraseDoublons(outputX, outputY);

	SKA_DBG_ONLY(
		if (!outputX.empty()) {
			SKA_LOG_INFO("X blocks : ", outputX.size());
		}

		if (!outputY.empty()) {
			SKA_LOG_INFO("Y blocks : ", outputY.size());
		}
		
		if(col) {
			SKA_LOG_INFO("Hitbox : (", hitbox.x, " ; ", hitbox.y, " ; ", hitbox.w, " ; ", hitbox.h, ")");
		}
	);

	return col;*/
	return false;
}

void ska::World::graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {	
	for (const auto& graphicLayer : m_graphicLayers) {
		drawables.add(*graphicLayer);
	}
}

const ska::Rectangle* ska::World::getView() const {
	return m_cameraSystem == nullptr ? nullptr : m_cameraSystem->getDisplay();
}

ska::Layer& ska::World::loadLayer(const std::string& layerFileName){
	if(m_chipset == nullptr) {
		throw IllegalStateException("Load a chipset before loading a layer");
	}
	ska::LayerLoader layerLoader;
	auto layerData = layerLoader.load(layerFileName, *m_chipset);

	auto l = std::make_unique<ska::Layer>(std::move(layerData.physics));
	auto& result = m_collisionProfile.addLayer(std::move(l));

	auto lGraphics = std::make_unique<ska::LayerRenderable>(std::move(layerData.graphics), m_chipset->getRenderable(), m_blockSize);
	m_graphicLayers.push_back(std::move(lGraphics));
	return result;
}

void ska::World::load(const std::string& fileName, const std::string& chipsetName) {
	m_autoScriptsPlayed = false;
	
	const auto chipsetChanged = m_chipset == nullptr ? true : m_chipset->getName() != chipsetName;
	if (chipsetChanged) {
		m_chipset = std::make_unique<Chipset>(m_correspondanceMapper, m_blockSize, chipsetName);
		m_chipsetEvent = std::make_unique<ChipsetEvent>(chipsetName);
	}

	const auto worldChanged = fileName != m_fileName;
	if (worldChanged) {
		const ska::FileNameData fndata(fileName);
		m_genericName = fndata.name;
		m_worldName = fndata.path + "/" + fndata.name;
		m_fileName = fileName;
	}

	if (worldChanged || chipsetChanged) {
		const auto fileNamePrefix = m_worldName + std::string(FILE_SEPARATOR) + m_genericName;
		const auto& botLayerName = fileNamePrefix + ".bmp";
		const auto& midLayerName = fileNamePrefix + "M.bmp";
		const auto& topLayerName = fileNamePrefix + "T.bmp";
		const auto& eventLayerName = fileNamePrefix + "E.txt";

		m_collisionProfile.clear();
		m_graphicLayers.clear();

		loadLayer(botLayerName);
		loadLayer(midLayerName);
		auto topLayer = loadLayer(topLayerName);

		m_nbrBlockX = topLayer.getBlocksX();
		m_nbrBlockY = topLayer.getBlocksY();

		m_layerE.changeLevel(eventLayerName);

		if (m_cameraSystem != nullptr) {
			m_cameraSystem->worldResized(getPixelWidth(), getPixelHeight());
		}
	}

}

std::vector<ska::ScriptSleepComponent*> ska::World::chipsetScript(const Point<int>& oldPos, const Point<int>& newPos, const Point<int>& posToLookAt, const ScriptTriggerType& reason, const unsigned int layerIndex) {
	std::vector<ScriptSleepComponent*> result;
	if (m_chipsetEvent == nullptr) {
		return result;
	}

	if (reason == EnumScriptTriggerType::AUTO) {
		auto tmp = m_chipsetEvent->getScript("", reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->context = m_worldName;
				result.push_back(ssc);
			}
		}
		return result;
	}

	const auto newBlock = newPos / m_blockSize;
	const auto oldBlock = oldPos / m_blockSize;
	const auto& b = m_collisionProfile.getBlock(layerIndex, posToLookAt.x / m_blockSize, posToLookAt.y / m_blockSize);
	if (b != nullptr) {
		const auto id = b->getID();
		auto tmp = m_chipsetEvent->getScript(StringUtils::intToStr(id), reason, m_autoScriptsPlayed);
		for (auto& ssc : tmp) {
			if (ssc != nullptr) {
				ssc->args.clear();
				ssc->args.push_back(StringUtils::intToStr(oldBlock.x));
				ssc->args.push_back(StringUtils::intToStr(oldBlock.y));
				ssc->args.push_back(StringUtils::intToStr(newBlock.x));
				ssc->args.push_back(StringUtils::intToStr(newBlock.y));
				ssc->args.push_back(StringUtils::intToStr(RectangleUtils::getDirectionFromPos(oldBlock * m_blockSize, newBlock * m_blockSize)));
				ssc->context = m_worldName;
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




