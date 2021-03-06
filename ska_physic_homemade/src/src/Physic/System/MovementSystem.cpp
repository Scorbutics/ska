#include "Physic/__internalConfig/LoggerConfig.h"
#include "MovementSystem.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/Ticked.h"

ska::MovementSystem::MovementSystem(ska::EntityManager& entityManager) :
	System(entityManager) {
}

void ska::MovementSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& posComponent = m_componentAccessor.get<PositionComponent>(entityId);
		auto& moveComponent = m_componentAccessor.get<MovementComponent>(entityId);
		auto& forceComponent = m_componentAccessor.get<ForceComponent>(entityId);

		auto lastMoveComponent = moveComponent;

		//Position vector is the current position of the object (i.e. the last position + the last velocity with a delta time taken)
		//(x(t) - x(t-1))/(t - (t-1)) = dx/dt (t) = vx(t)
		posComponent.x += static_cast<long>(moveComponent.vx);
		posComponent.y += static_cast<long>(moveComponent.vy);
		posComponent.z += static_cast<long>(moveComponent.vz);

		//Acceleration vector ALWAYS means the next acceleration, and not the current one
		/* sum(F) = m*a */
		moveComponent.ax = (forceComponent.x / forceComponent.weight);
		moveComponent.ay = (forceComponent.y / forceComponent.weight);
		moveComponent.az = (forceComponent.z / forceComponent.weight);

		//Velocity vector ALWAYS means the NEXT velocity and not the current one
		//(v(t) - v(t-1))/(t - (t-1)) = dv/dt (t) = a(t)
		moveComponent.vx += (moveComponent.ax + lastMoveComponent.ax) / 2;
		moveComponent.vy += (moveComponent.ay + lastMoveComponent.ay) / 2;
		moveComponent.vz += (moveComponent.az + lastMoveComponent.az) / 2;

		/* (Don't forget to reset the current forces applied to, because once forces are used, they do) */
		forceComponent.x = 0;
		forceComponent.y = 0;
		forceComponent.z = 0;

		//The acceleration, velocity and position order is important, as they are used in other systems.
		//So velocity and acceleration have to ALWAYS BE the NEXT one.

		/* Ground reflection */
		if (posComponent.z < 0 || (posComponent.z + moveComponent.vz) <= 0) {
			posComponent.z = 0;
			moveComponent.vz = 0;
		}

		/* Ignore low values */
		/*if (NumberUtils::absolute(moveComponent.vx) < 0.001) {
			moveComponent.vx = 0;
		}

		if (NumberUtils::absolute(moveComponent.vy) < 0.001) {
			moveComponent.vy = 0;
		}

		if (NumberUtils::absolute(moveComponent.vz) < 0.001) {
			moveComponent.vz = 0;
		}*/
	}
}

ska::MovementSystem::~MovementSystem()
{
}
