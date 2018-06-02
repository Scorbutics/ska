#include "EntityCollisionResponse.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/EntityManager.h"
#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "Utils/RectangleUtils.h"

ska::EntityCollisionResponse::EntityCollisionResponse(GameEventDispatcher& ged, EntityManager& em) :
	EntityCollisionObserver(std::bind(&EntityCollisionResponse::onEntityCollision, this, std::placeholders::_1), ged),
	m_entityManager(em) {
}

ska::EntityCollisionResponse::EntityCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, GameEventDispatcher& ged, EntityManager& em) :
    EntityCollisionObserver(onEntityCollision, ged),
    m_entityManager(em) {
}

void ska::EntityCollisionResponse::correctPosition(ska::PositionComponent& origin, ska::PositionComponent& target, const float invMassOrigin, const float invMassTarget, const CollisionContact& cc, const float slope, const float percent) {
	const auto penetration = cc.penetration();
	const auto& n = cc.normal();
	
	const auto constCorrection = ska::NumberUtils::maximum(penetration - slope, 0.0f) / (invMassOrigin + invMassTarget) * percent;
	
	auto correction = ska::Point<float>{ constCorrection * n.x , constCorrection * n.y };
	target.x -= invMassTarget * correction.x;
	target.y -= invMassTarget * correction.y;
	origin.x += invMassOrigin * correction.x;
	origin.y += invMassOrigin * correction.y;
}

bool ska::EntityCollisionResponse::handleInfiniteMasses(const CollisionComponent& col, float invMassOrigin, float invMassTarget, MovementComponent& mtarget, MovementComponent& morigin) {
	if (invMassOrigin == 0 && invMassTarget == 0) {
		mtarget.vx = 0;
		mtarget.vy = 0;
		morigin.vx = 0;
		morigin.vy = 0;
		return true;
	}
	return false;
}

bool ska::EntityCollisionResponse::onEntityCollision(CollisionEvent& e) {
	if (e.collisionComponent == nullptr) {
		return false;
	}

	auto& col = *e.collisionComponent;

	auto& mtarget = m_entityManager.getComponent<MovementComponent>(col.target);
	auto& morigin = m_entityManager.getComponent<MovementComponent>(col.origin);

	auto& ftarget = m_entityManager.getComponent<ForceComponent>(col.target);
	auto& forigin = m_entityManager.getComponent<ForceComponent>(col.origin);

	auto bounciness = std::min(forigin.bounciness, ftarget.bounciness);
	bounciness = bounciness < 0.F ? -bounciness : bounciness;

	const auto invMassOrigin = forigin.weight == std::numeric_limits<double>::max() ? 0 : 1.0 / forigin.weight;
	const auto invMassTarget = ftarget.weight == std::numeric_limits<double>::max() ? 0 : 1.0 / ftarget.weight;

	if(handleInfiniteMasses(col, invMassOrigin, invMassTarget, mtarget, morigin)) {
		return true;
	}

	const Point<float> velocityDiffVector(mtarget.vx - morigin.vx, mtarget.vy - morigin.vy);
	const auto diffVelocityOnNormal = RectangleUtils::projection(velocityDiffVector, col.contact.normal());
	const auto j = (-(1.0 + bounciness) * (diffVelocityOnNormal)) / (invMassOrigin + invMassTarget) ;

	/*SKA_DBG_ONLY(
		//if (ska::NumberUtils::absolute(j) > 0.0001) {
			auto roundedVelocityTarget = ska::Point<float>(mtarget.vx, mtarget.vy);
			SKA_LOG_INFO("velocity = ", roundedVelocityTarget.x, ";", roundedVelocityTarget.y, "\tJ coeff = ", j);
		//}
	);*/

	//calcul vectoriel : impulse = j . normal
	const auto impulse = Point<double>{ j * col.contact.normal().x, j * col.contact.normal().y };

	mtarget.vx += impulse.x * invMassTarget;
	mtarget.vy += impulse.y * invMassTarget;
	
	morigin.vx += -impulse.x * invMassOrigin;	
	morigin.vy += -impulse.y * invMassOrigin;

	//usually 20% to 80%
	static const auto percent = 0.2F;

	//usually 0.01 to 0.1
	static const auto slope = 0.1F;
	correctPosition(m_entityManager.getComponent<PositionComponent>(col.origin), m_entityManager.getComponent<PositionComponent>(col.target), invMassOrigin, invMassTarget, col.contact, slope, percent);

	return true;
}

ska::EntityCollisionResponse::~EntityCollisionResponse() {
}
