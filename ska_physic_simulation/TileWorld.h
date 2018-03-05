#pragma once
#include "../ska_world/World/World.h"

class World : 
	public ska::TileWorld {
public:
	World();
	virtual void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual ~World() override = default;

private:

};
