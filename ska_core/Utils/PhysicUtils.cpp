#include "PhysicUtils.h"

constexpr static auto PI = M_PI;

ska::PhysicUtils::PhysicUtils()
{
}

const ska::Force ska::PhysicUtils::getMovement(const int direction, const float speed) {
	float angle;
	Force f;

	switch (direction)
	{
	case 0:
		angle = (float)PI / 2;
		break;

	case 1:
		angle = (float)0.;
		break;

	case 2:
		angle = -(float)PI / 2;
		break;

	case 3:
		angle = -(float)PI;
		break;

	case 4:
		angle = (float)PI / 4;
		break;

	case 5:
		angle = -(float)PI / 4;
		break;

	case 6:
		angle = (float)(-3 * PI / 4);
		break;

	case 7:
		angle = (float)(3 * PI / 4);
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
