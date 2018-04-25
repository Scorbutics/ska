#pragma once
#include <unordered_set>
#include "Point.h"

namespace ska {
    class BlockAllowance {
    public:
        BlockAllowance() = default;
        virtual ~BlockAllowance() = default;

        virtual bool getCollision(std::size_t x, std::size_t y) const = 0;
        virtual unsigned int getBlockSize() const = 0;
        virtual bool isSameBlockId(const ska::Point<int>& centerPos, const ska::Point<int>& oldCenterPos, int) const = 0;
        virtual bool isBlockAuthorizedAtPos(const Point<int>& pos, const std::unordered_set<int>& blocksAuthorized) const = 0;
    };
}
