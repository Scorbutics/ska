#include "SpaceCollisionEventSender.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Data/Events/CollisionEvent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Data/Events/GameEventDispatcher.h"

ska::WorldCollisionComponent CreateWorldCollisionFromArbiter(ska::cp::Space& space, ska::cp::Arbiter& arb, ska::EntityId* entityId, unsigned int blockSize) {

	//std::cout << "Point A " << " : " << cpSet.points[0].pointA.x << "; " << cpSet.points[0].pointA.y << std::endl;
	//std::cout << "Point B " << " : " << cpSet.points[0].pointB.x << "; " << cpSet.points[0].pointB.y << std::endl;
	
	auto[bodyA, bodyB] = arb.getBodies();
	const auto cpSet = arb.getContactPoints();

	auto uniqueBlockPoints = std::unordered_set<ska::Point<int>>{};
	std::optional<ska::Point<int>> lastBlockPoint;
	for (auto i = 0; i < cpSet.count; i++) {
		ska::Point<int> blockPoint;
		if (bodyA != &space.getStaticBody()) {
			blockPoint = ska::Point<int>{ static_cast<int>(cpSet.points[i].pointA.x / blockSize), static_cast<int>(cpSet.points[i].pointA.y / blockSize) };
		} else {
			blockPoint = ska::Point<int>{ static_cast<int>(cpSet.points[i].pointB.x / blockSize), static_cast<int>(cpSet.points[i].pointB.y / blockSize) };
		}

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
	for (const auto& blockPoint : uniqueBlockPoints) {
		wcc.blockContacts.push_back(blockPoint);
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
