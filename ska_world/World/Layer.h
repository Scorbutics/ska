#pragma once

#include <string>
#include <memory>

#include "LayerRenderable.h"
#include "Physic/Block.h"
#include "Utils/Vector2.h"

class ParticleManager;


namespace ska {
	class World;
	class Layer {
	public:
		Layer(World& world, const std::string& nomFichier, Layer* parent = nullptr);
		Layer(World& w, Layer* parent = nullptr);
		void operator=(const Layer&) = delete;
		~Layer() = default;

		LayerRenderable& getRenderable();

		void reset(const std::string& file);

		void clear();

		Block* getBlock(unsigned int i, unsigned int j) const;
		int getBlockCollision(const unsigned int i, const unsigned int j) const;
		Layer* getParent() const;
		
		unsigned int getBlocksX() const;
		unsigned int getBlocksY() const;
		
	private:
		void checkSize(unsigned int nbrBlocX, unsigned int nbrBlocY) const;

		Layer* m_parent;
		World& m_world;
		std::string m_name, m_nomFichier;
		Vector2<Block*> m_block;
		LayerRenderable m_renderable;
		unsigned int m_fileWidth, m_fileHeight;

	};

	using LayerPtr = std::unique_ptr<Layer>;

}

