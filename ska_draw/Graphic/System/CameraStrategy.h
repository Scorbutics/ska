#pragma once
#include <unordered_set>
#include <memory>
#include "Rectangle.h"
#include "ECS/ECSDefines.h"

namespace ska {
	class CameraStrategy {
	public:
		CameraStrategy() = default;
		virtual ~CameraStrategy() = default;
		virtual std::pair<Rectangle, EntityId*> focus(const std::unordered_set<EntityId>& processed) = 0;
	};
	using CameraStrategyPtr = std::unique_ptr<CameraStrategy>;
}
