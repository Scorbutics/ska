#include "EntityCollisionResponse.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/EntityManager.h"
#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "Utils/RectangleUtils.h"

ska::EntityCollisionResponse::EntityCollisionResponse(GameEventDispatcher& ged, EntityManager& em) :
	EntityCollisionObserver(bind(&EntityCollisionResponse::onEntityCollision, this, std::placeholders::_1)),
	m_ged(ged),
	m_entityManager(em) {
	m_ged.ska::Observable<CollisionEvent>::addObserver(*this);
}

ska::EntityCollisionResponse::EntityCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, GameEventDispatcher& ged, EntityManager& em) :
    EntityCollisionObserver(onEntityCollision),
    m_ged(ged),
    m_entityManager(em) {
	m_ged.ska::Observable<CollisionEvent>::addObserver(*this);
}

bool ska::EntityCollisionResponse::calculateNormalAndPenetration(ska::CollisionComponent& col) const {
	auto& a = col.origin;
	auto& b = col.target;

	auto& pcA = m_entityManager.getComponent<ska::PositionComponent>(a);
	auto& pcB = m_entityManager.getComponent<ska::PositionComponent>(b);

	const auto& intersection = col.overlap;
	if (intersection.w < intersection.h) {
		auto& normal = col.normal;
		const auto vectorAToBX = pcA.x - pcB.x;
		normal.x = vectorAToBX < 0  ? -1.F : 1.F;
		normal.y = 0;
		col.penetration = static_cast<float>(intersection.w);
	} else {
		auto& normal = col.normal;
		const auto vectorAToBY = pcA.y - pcB.y;
		normal.x = 0;
		normal.y = vectorAToBY < 0  ? -1.F : 1.F;
		col.penetration = static_cast<float>(intersection.h);
	}
	return true;
}

void ska::EntityCollisionResponse::correctPosition(ska::PositionComponent& origin, ska::PositionComponent& target, float invMassOrigin, float invMassTarget, float penetration, ska::Point<float>& n) {
	const auto percent = 0.1F;
	const auto slope = 0.005F;

	ska::Point<float> correction;
	const auto constCorrection = ska::NumberUtils::maximum(penetration - slope, 0.0f) / (invMassOrigin + invMassTarget) * percent;
	correction.x = constCorrection * n.x;
	correction.y = constCorrection * n.y;
	target.x -= invMassTarget * correction.x;
	target.y -= invMassTarget * correction.y;
	origin.x += invMassOrigin * correction.x;
	origin.y += invMassOrigin * correction.y;
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

	if(invMassOrigin == 0 && invMassTarget == 0) {
        if (col.xaxis) {
            mtarget.vx = 0;
            morigin.vx = 0;
        }

        if (col.yaxis) {
            mtarget.vy = 0;
            morigin.vy = 0;
        }
        return true;
	}

	const Point<float> velocityDiffVector(mtarget.vx - morigin.vx, mtarget.vy - morigin.vy);
	const auto diffVelocityOnNormal = RectangleUtils::projection(velocityDiffVector, col.normal);
	const auto j = (-(1 + bounciness) * diffVelocityOnNormal) / (invMassOrigin + invMassTarget) ;

	//impulse = j . normal
	Point<float> impulse(j * col.normal.x, j * col.normal.y);

	if (col.xaxis) {
		mtarget.vx += impulse.x * invMassTarget;
		morigin.vx += -impulse.x * invMassOrigin;
	}

	if (col.yaxis) {
		mtarget.vy += impulse.y * invMassTarget;
		morigin.vy += -impulse.y * invMassOrigin;
	}

	correctPosition(m_entityManager.getComponent<PositionComponent>(col.origin), m_entityManager.getComponent<PositionComponent>(col.target), invMassOrigin, invMassTarget, col.penetration, col.normal);

	return true;
}

ska::EntityCollisionResponse::~EntityCollisionResponse() {
	m_ged.ska::Observable<CollisionEvent>::removeObserver(*this);
}
