#pragma once
#include "../../Component.h"
#include "../../../Utils/StringUtils.h"
#include "../../../Utils/PhysicUtils.h"
#include "../../../Point.h"
#include "HitboxComponent.h"
#include "../Graphic/AnimationComponent.h"
#include "../../../Utils/NumberUtils.h"

namespace ska {
	class PositionComponent : public Component {
	public:
		PositionComponent();
		PositionComponent(const Point<float>& p);
		PositionComponent(const Point<int>& p);
        void operator=(const Point<float>& p);

		static Point<float> getCenterPosition(const PositionComponent& pc, const HitboxComponent& hc) {
			Point<float> result;
			result.x = static_cast<float>(((pc.x + hc.xOffset) + (pc.x + hc.xOffset + hc.width) + 1) / 2);
			result.y = static_cast<float>(((pc.y + hc.yOffset) + (pc.y + hc.yOffset + hc.height) + 1) / 2);
			return result;
		}

		//TODO move vers ska_physic
		static Point<float> getFrontPosition(const PositionComponent& pc, const HitboxComponent& hc, const AnimationComponent& dac) {
			Point<float> result;
			Rectangle absoluteHitbox;
			absoluteHitbox.x = static_cast<int>((pc.x + static_cast<float>(hc.xOffset)));
			absoluteHitbox.y = static_cast<int>((pc.y + static_cast<float>(hc.yOffset)));
			absoluteHitbox.w = static_cast<int>(hc.width);
			absoluteHitbox.h = static_cast<int>(hc.height);
			result.x = (absoluteHitbox.x + (absoluteHitbox.x + absoluteHitbox.w) + 1) / 2.F;
			result.y = (absoluteHitbox.y + (absoluteHitbox.y + absoluteHitbox.h) + 1) / 2.F;

			auto fDir = PhysicUtils::getMovement(dac.state, NumberUtils::maximum(static_cast<float>(absoluteHitbox.w), static_cast<float>(absoluteHitbox.h)));
			auto pos = Point<float>::cartesian(fDir.power, fDir.angle);
			return result + pos;
		}

	protected:
		static std::string serializeX(const Component& component) {
			return StringUtils::intToStr(static_cast<int>(static_cast<const PositionComponent&>(component).x));
		}

		static std::string serializeY(const Component& component) {
			return StringUtils::intToStr(static_cast<int>(static_cast<const PositionComponent&>(component).y));
		}

		static std::string serializeZ(const Component& component) {
			return StringUtils::intToStr(static_cast<int>(static_cast<const PositionComponent&>(component).z));
		}

	public:
		float x;
		float y;
		float z;
	};
}

