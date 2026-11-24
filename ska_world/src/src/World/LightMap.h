#pragma once
#include <memory>
#include <string>
#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/Rectangle.h"

namespace ska {
	class DrawableContainer;
	class Texture;

	/**
	 * \brief The light of a map : soft shade and light over its blocks, like the gradients of a 3D scene.
	 *
	 * An image of (blocks wide + 1) x (blocks high + 1) pixels : one value per corner of the blocks. It is stretched
	 * smoothly over the map (linear filtering : a gradient from one corner to the next, across the blocks) and
	 * multiplied with what is drawn under it (white : unchanged, grey : shade, a tint : the colour of the light).
	 * A map without its light image is drawn as it is.
	 * Its priority places it in the drawings (ex : over the ground and the characters, under the interface).
	 */
	class LightMap :
		public DrawableFixedPriority {
	public:
		LightMap();
		~LightMap() override;

		/**
		 * \brief Loads the light of a map, or none when the file does not exist.
		 * \param file The image (PNG), one pixel per corner of the blocks
		 * \param blockSize The size of a block, in pixels
		 * \param blocksX, blocksY The size of the map, in blocks (the image : one pixel more each way)
		 */
		void load(const std::string& file, unsigned int blockSize, unsigned int blocksX, unsigned int blocksY);
		bool loaded() const;
		const std::string& file() const;

		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables);
		void render(Renderer& renderer) const override;

	private:
		std::unique_ptr<Texture> m_texture;
		std::string m_file;
		unsigned int m_blockSize = 0;
		Rectangle m_cameraPos{};
	};
}
