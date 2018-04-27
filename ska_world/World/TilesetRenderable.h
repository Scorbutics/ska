#pragma once
#include <optional>
#include "Graphic/Texture.h"
#include "Utils/Vector2.h"
#include "Graphic/Animation.h"

namespace ska {
	class TilesetLoader;

	class TilesetRenderable : 
		public MovableNonCopyable {
	public:
		explicit TilesetRenderable(unsigned int tileSize, const TilesetLoader& loader);
		TilesetRenderable(TilesetRenderable&&) = default;

		void render(const Renderer& renderer, const Point<int>& pos, const Point<int>& tileId) const;
		const Texture& getTexture() const;
		std::optional<Animation>& getAnimation(const Point<int>& pos);

		~TilesetRenderable() override = default;

	private:
		void load(const TilesetLoader& loader);

		mutable Vector2<std::optional<Animation>> m_animations;
		Texture m_tileset;

		const std::string m_tilesetName;
		const unsigned int m_tileSize;
	};
}
