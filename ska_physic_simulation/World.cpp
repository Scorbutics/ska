#include "World.h"

bool World::intersectBlocksAtPos(const ska::Rectangle& hitbox, std::vector<ska::Point<int>>& outputX, std::vector<ska::Point<int>>& outputY) const {
	return false;
}

bool World::isBlockAuthorizedAtPos(const ska::Point<int>& pos, const std::unordered_set<int>& blocksAuthorized) const {
	return true;
}
