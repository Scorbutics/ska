#include "SpaceCollisionEventSender.h"
#include "Utils/RectangleUtils.h"
#include "Utils/NumberUtils.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Data/Events/CollisionEvent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Data/Events/GameEventDispatcher.h"
#include "PredefinedCollisionTypes.h"
#include "../CollisionHandlerType.h"

ska::WorldCollisionComponent CreateWorldCollisionFromArbiter(ska::cp::Space& space, ska::cp::Arbiter& arb, ska::EntityId* entityId, unsigned int blockSize) {
	auto[bodyA, bodyB] = arb.getBodies();
	const auto cpSet = arb.getContactPoints();

	auto uniqueBlockPoints = std::unordered_set<ska::Point<int>>{};
	std::optional<ska::Point<int>> lastBlockPoint;
	
	for (auto i = 0; i < cpSet.count; i++) {
		cpVect bodyBlock;
		cpVect bodyEntity;
		
		if (bodyA == &space.getStaticBody()) {
			bodyBlock = cpSet.points[i].pointB;
			bodyEntity = cpSet.points[i].pointA;
		} else {
			bodyBlock = cpSet.points[i].pointA;
			bodyEntity = cpSet.points[i].pointB;
		}
		
		bodyBlock.x = bodyBlock.x < bodyEntity.x ? ska::NumberUtils::round(bodyBlock.x - blockSize / 2) : ska::NumberUtils::round(bodyBlock.x + blockSize / 2);
		bodyBlock.y = bodyBlock.y < bodyEntity.y ? ska::NumberUtils::round(bodyBlock.y - blockSize / 2) : ska::NumberUtils::round(bodyBlock.y + blockSize / 2);

		//SKA_STATIC_LOG_DEBUG(ska::cp::SpaceCollisionEventSender)("Blocks collision positions : block ", bodyBlock.x, "; ", bodyBlock.y, " entity ", bodyEntity.x, "; ", bodyEntity.y);

		auto blockPoint = ska::Point<int> { 
			static_cast<int>(bodyBlock.x / blockSize),
			static_cast<int>(bodyBlock.y / blockSize)
		};

		//When possible, manages intermediate blocks
		if (lastBlockPoint.has_value()) {
			const auto intermediates = ska::RectangleUtils::createIntermediatePoints(*lastBlockPoint, blockPoint);
			for (const auto& i : intermediates) {
				uniqueBlockPoints.insert(i);
			}
		}

		uniqueBlockPoints.insert(blockPoint);
		lastBlockPoint = blockPoint;
	}

	auto wcc = ska::WorldCollisionComponent{};
	//SKA_STATIC_LOG_DEBUG(ska::cp::SpaceCollisionEventSender)("Blocks collision");
	for (const auto& blockPoint : uniqueBlockPoints) {
		wcc.blockContacts.push_back(blockPoint * blockSize);
		//SKA_STATIC_LOG_DEBUG(ska::cp::SpaceCollisionEventSender)(blockPoint.x, "; ", blockPoint.y);
	}

	wcc.normal = ska::Point<float>{
		static_cast<float>(cpSet.normal.x),
		static_cast<float>(cpSet.normal.y)
	};
	return wcc;
}

ska::cp::SpaceCollisionEventSender::SpaceCollisionEventSender(Space& space, GameEventDispatcher& ged, unsigned blockSize) :
	m_space(space) {
	m_callbackIndex = m_space.addDefaultCollisionCallback<CollisionHandlerType::BEGIN>([&ged, blockSize](ska::cp::Arbiter& arb, ska::cp::Space& space, ska::EntityId* entityId) {
		auto[bodyA, bodyB] = arb.getBodies();
		const auto isWorldCollision = bodyA == &space.getStaticBody() || bodyB == &space.getStaticBody();

		if (entityId != nullptr && isWorldCollision) {
			auto wcc = CreateWorldCollisionFromArbiter(space, arb, entityId, blockSize);

			const auto colliComp = ska::CollidableComponent{};
			auto ce = CollisionEvent{ *entityId, &wcc, nullptr, colliComp };
			ged.Observable<CollisionEvent>::notifyObservers(ce);
		}
		return true;
	});

}

ska::cp::SpaceCollisionEventSender::~SpaceCollisionEventSender() {
	m_space.removeDefaultCollisionCallback<CollisionHandlerType::BEGIN>(m_callbackIndex);
}
