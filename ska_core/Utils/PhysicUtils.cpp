#include "PhysicUtils.h"

constexpr static auto PI = 3.14159265359f;

ska::PhysicUtils::PhysicUtils()
{
}

const ska::Force ska::PhysicUtils::getMovement(const int direction, const float speed) {
	float angle;
	Force f;

	switch (direction)
	{
	case 0:
		angle = PI / 2;
		break;

	case 1:
		angle = 0.F;
		break;

	case 2:
		angle = -PI / 2;
		break;

	case 3:
		angle = -PI;
		break;

	case 4:
		angle = PI / 4;
		break;

	case 5:
		angle = -PI / 4;
		break;

	case 6:
		angle = (-3 * PI / 4);
		break;

	case 7:
		angle = (3 * PI / 4);
		break;

	default:
		angle = 0;
		break;
	}

	f.angle = (angle);
	f.power = (speed);
	return f;
}

ska::PhysicUtils::~PhysicUtils()
{
}
