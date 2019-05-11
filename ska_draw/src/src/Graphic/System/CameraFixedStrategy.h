#pragma once
#include "CameraStrategy.h"
#include "Core/Point.h"

namespace ska {
	class CameraFixedStrategy : 
		public CameraStrategy {
	public:
		CameraFixedStrategy(const ska::Point<int>& pos);
		~CameraFixedStrategy() override = default;
		
		std::pair<Rectangle, EntityId*> focus(const std::unordered_set<EntityId>& processed) override;
	
	private:
		ska::Point<int> m_pos;
	};
}