#include "Block.h"

ska::Block::Block(int corrFileWidth, Point<int> posChipset, unsigned int properties, BlockCollision col) {
	m_properties = properties;
    m_collision = col;
	m_id = posChipset.x + posChipset.y * corrFileWidth;
}

unsigned int ska::Block::getProperties() const{
	return m_properties;
}

ska::BlockCollision ska::Block::getCollision() const {
    return m_collision;
}

unsigned int ska::Block::getID() const {
	return m_id;
}
