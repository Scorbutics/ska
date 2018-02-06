#pragma once
#include <vector>
#include <unordered_set>
#include "Rectangle.h"
#include "Point.h"

namespace ska {
    class CollisionProfile {
    public:
        CollisionProfile() = default;
        virtual ~CollisionProfile() = default;

        virtual bool intersectBlocksAtPos(const Rectangle& hitbox, std::vector<Rectangle>& outputX, std::vector<Rectangle>& outputY) const = 0;
        virtual bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& blocksAuthorized) const = 0;
    };
}
