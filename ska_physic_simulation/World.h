#pragma once
#include "ECS/Basics/Physic/CollisionProfile.h"

class World : 
	public ska::CollisionProfile {
public:
	virtual ~World() override = default;
	virtual bool intersectBlocksAtPos(const ska::Rectangle& hitbox, std::vector<ska::Point<int>>& outputX, std::vector<ska::Point<int>>& outputY) const override;
	virtual bool canMoveOnBlock(const ska::Point<int>& pos, const std::unordered_set<int>& blocksAuthorized, int i) const override;

private:

};
