#include "EntityCollisionResponse.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/EntityManager.h"
#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "Utils/RectangleUtils.h"

ska::EntityCollisionResponse::EntityCollisionResponse(GameEventDispatcher& ged, EntityManager& em) :
	EntityCollisionObserver(bind(&EntityCollisionResponse::onEntityCollision, this, std::placeholders::_1)),
	m_entityManager(em),
	m_ged(ged) {
	m_ged.ska::Observable<CollisionEvent>::addObserver(*this);
}

ska::EntityCollisionResponse::EntityCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, GameEventDispatcher& ged, EntityManager& em) :
EntityCollisionObserver(onEntityCollision),
m_entityManager(em),
m_ged(ged) {
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

	const auto invMassOrigin = 1 / forigin.weight;
	const auto invMassTarget = 1 / ftarget.weight;

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

	return true;
}

ska::EntityCollisionResponse::~EntityCollisionResponse() {
	m_ged.ska::Observable<CollisionEvent>::removeObserver(*this);
}
