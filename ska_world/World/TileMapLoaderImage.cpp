#include "TileMapLoaderImage.h"
#include "Layer.h"
#include "LayerLoader.h"
#include "LayerRenderable.h"
#include "TileWorld.h"

ska::TileMapLoaderImage::TileMapLoaderImage(std::string levelPath, std::string chipsetPath, std::string chipsetCorrespondanceFileName) :
	m_levelPath(std::move(levelPath)),
	m_chipsetPath(std::move(chipsetPath)),
	m_chipsetCorrespondanceFileName(std::move(chipsetCorrespondanceFileName)) {
}

std::pair<std::unique_ptr<ska::Layer>, std::unique_ptr<ska::LayerRenderable>> ska::TileMapLoaderImage::loadLayer(const int blockSize, Tileset& tileset, const std::string& layerFileName) {
	ska::LayerLoader layerLoader;
	auto layerData = layerLoader.load(layerFileName, tileset);

	auto l = std::make_unique<Layer>(std::move(layerData.physics));
	auto lGraphics = std::make_unique<LayerRenderable>(std::move(layerData.graphics), tileset.getRenderable(), blockSize);

	return std::make_pair(std::move(l), std::move(lGraphics));
}

std::unique_ptr<ska::Tileset> ska::TileMapLoaderImage::load(const int blockSize, CollisionProfile& profile, std::vector<LayerRenderablePtr>& renderables) {
	auto tileset = std::make_unique<Tileset>(TilesetCorrespondanceMapper{ m_chipsetCorrespondanceFileName }, blockSize, m_chipsetPath);

	const auto fileNamePrefix = m_levelPath + "/";
	const auto& botLayerName = fileNamePrefix + ".bmp";
	const auto& midLayerName = fileNamePrefix + "M.bmp";
	const auto& topLayerName = fileNamePrefix + "T.bmp";

	auto [botLayer, botLayerGraphics] = loadLayer(blockSize, *tileset, botLayerName);
	auto [midLayer, midLayerGraphics] = loadLayer(blockSize, *tileset, midLayerName);
	auto [topLayer, topLayerGraphics] = loadLayer(blockSize, *tileset, topLayerName);
	
	profile.addLayer(std::move(botLayer));
	profile.addLayer(std::move(midLayer));
	profile.addLayer(std::move(topLayer));

	renderables.push_back(std::move(botLayerGraphics));
	renderables.push_back(std::move(midLayerGraphics));
	renderables.push_back(std::move(topLayerGraphics));
	
	return std::move(tileset);
}
