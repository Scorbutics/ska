#include "EntityCollisionResponse.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/EntityManager.h"
#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "Utils/RectangleUtils.h"
#include "Core/CodeDebug/CodeDebug.h"

ska::EntityCollisionResponse::EntityCollisionResponse(GameEventDispatcher& ged, EntityManager& em) :
	EntityCollisionObserver(std::bind(&EntityCollisionResponse::onEntityCollision, this, std::placeholders::_1), ged),
	m_ged(ged),
	m_entityManager(em) {
}

ska::EntityCollisionResponse::EntityCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, GameEventDispatcher& ged, EntityManager& em) :
    EntityCollisionObserver(onEntityCollision, m_ged),
    m_ged(ged),
    m_entityManager(em) {
}

bool ska::EntityCollisionResponse::calculateNormalAndPenetration(ska::CollisionComponent& col) const {
	auto& a = col.origin;
	auto& b = col.target;

	auto& pcA = m_entityManager.getComponent<ska::PositionComponent>(a);
	auto& pcB = m_entityManager.getComponent<ska::PositionComponent>(b);

	auto& mcA = m_entityManager.getComponent<ska::MovementComponent>(a);
	auto& mcB = m_entityManager.getComponent<ska::MovementComponent>(b);

	const Point<float> absoluteDiffVelocity(ska::NumberUtils::absolute(mcA.vx - mcB.vx), ska::NumberUtils::absolute(mcA.vy - mcB.vy));

	const auto& intersection = col.overlap;
	if (ska::NumberUtils::absolute(intersection.h - absoluteDiffVelocity.y) < ska::NumberUtils::absolute(intersection.w - absoluteDiffVelocity.x)) {
		auto& normal = col.normal;
		const auto vectorAToBX = pcA.x - pcB.x;
		normal.x = vectorAToBX < 0  ? -1.F : 1.F;
		normal.y = 0;
		col.penetration = static_cast<float>(intersection.w);
		SKA_LOG_INFO("horizontal normal");
	} else {
		auto& normal = col.normal;
		const auto vectorAToBY = pcA.y - pcB.y;
		normal.x = 0;
		normal.y = vectorAToBY < 0  ? -1.F : 1.F;
		col.penetration = static_cast<float>(intersection.h);
		SKA_LOG_INFO("vertical normal");
	}
	return true;
}

void ska::EntityCollisionResponse::correctPosition(ska::PositionComponent& origin, ska::PositionComponent& target, float invMassOrigin, float invMassTarget, float penetration, ska::Point<float>& n) {
	const auto percent = 0.01F;
	const auto slope = 0.02F;

	ska::Point<float> correction;
	const auto constCorrection = ska::NumberUtils::maximum(penetration - slope, 0.0f) / (invMassOrigin + invMassTarget) * percent;
	correction.x = constCorrection * n.x;
	correction.y = constCorrection * n.y;
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

	const auto& bounciness = std::min(forigin.bounciness, ftarget.bounciness);

	calculateNormalAndPenetration(col);

	const auto invMassOrigin = forigin.weight == std::numeric_limits<float>::max() ? 0 : 1 / forigin.weight;
	const auto invMassTarget = ftarget.weight == std::numeric_limits<float>::max() ? 0 : 1 / ftarget.weight;

	if(handleInfiniteMasses(col, invMassOrigin, invMassTarget, mtarget, morigin)) {
		return true;
	}

	const Point<float> velocityDiffVector(mtarget.vx - morigin.vx, mtarget.vy - morigin.vy);
	const auto diffVelocityOnNormal = RectangleUtils::projection(velocityDiffVector, col.normal);
	const auto j = (-(1 + bounciness) * diffVelocityOnNormal) / (invMassOrigin + invMassTarget) ;

	/*SKA_DBG_ONLY(
		auto roundedVelocityTarget = ska::Point<float>(ska::NumberUtils::round(mtarget.vx), ska::NumberUtils::round(mtarget.vy));
		SKA_LOG_INFO("velocity = ", roundedVelocityTarget.x, ";", roundedVelocityTarget.y, "\tJ coeff = ", j);
	);*/

	//impulse = j . normal
	Point<float> impulse(j * col.normal.x, j * col.normal.y);

	mtarget.vx += impulse.x * invMassTarget;
	mtarget.vy += impulse.y * invMassTarget;
	
	morigin.vx += -impulse.x * invMassOrigin;	
	morigin.vy += -impulse.y * invMassOrigin;

	correctPosition(m_entityManager.getComponent<PositionComponent>(col.origin), m_entityManager.getComponent<PositionComponent>(col.target), invMassOrigin, invMassTarget, col.penetration, col.normal);

	return true;
}

ska::EntityCollisionResponse::~EntityCollisionResponse() {
}
