#pragma once
#include <optional>
#include "Graphic/Texture.h"
#include "Base/Containers/Vector2.h"
#include "Graphic/Animation.h"
#include "TileAnimation.h"

namespace ska {
	class TilesetLoader;

	class TilesetRenderable : 
		public MovableNonCopyable {
	public:
		explicit TilesetRenderable(unsigned int tileSize, const TilesetLoader& loader);
		TilesetRenderable(TilesetRenderable&&) = default;

		const Texture& getTexture() const;
		TileAnimation& getAnimation(const Point<int>& pos);
		const TileAnimation& getAnimation(const Point<int>& pos) const;

		void update();

		~TilesetRenderable() override = default;

	private:
		void load(const TilesetLoader& loader);

		mutable Vector2<TileAnimation> m_animations;
		Texture m_tileset;

		const std::string m_tilesetName;
		const unsigned int m_tileSize;
		Vector2<std::reference_wrapper<Animation>> m_animatedAnimations;
	};
}
