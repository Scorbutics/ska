#include "World.h"
#include "Draw/DrawableContainer.h"

World::World() :
	ska::World(48) {
	
}

void World::graphicUpdate(unsigned ellapsedTime, ska::DrawableContainer& drawables) {
	drawables.add(m_lBot.getRenderable());
	drawables.add(m_lMid.getRenderable());
	drawables.add(m_lTop.getRenderable());
}

