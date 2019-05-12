#include "Physic/__internalConfig/LoggerConfig.h"
#include "SpaceCollisionEventSender.h"
#include "Core/Utils/RectangleUtils.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/Data/Events/CollisionEvent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/ECS/EntityManager.h"
#include "Core/Data/Events/GameEventDispatcher.h"
#include "PredefinedCollisionTypes.h"
#include "../CollisionHandlerType.h"

namespace ska {
	namespace cp {
		
		std::tuple<cpVect, cpVect, Point<int>> ExtractBlockAndEntityFromContact(Space& space, const cpContactPointSet& cpSet, cpBody* body, const std::size_t index) {
			cpVect bodyBlock;
			cpVect bodyEntity;

			int sign;
			if (body == &space.getStaticBody()) {
				sign = 1;
				bodyBlock = cpSet.points[index].pointB;
				bodyEntity = cpSet.points[index].pointA;
			} else {
				sign = -1;
				bodyBlock = cpSet.points[index].pointA;
				bodyEntity = cpSet.points[index].pointB;
			}
			return std::make_tuple(bodyBlock, bodyEntity, cpSet.normal * sign);
		}
	}
}

ska::WorldCollisionComponent ska::cp::SpaceCollisionEventSender::createWorldCollisionFromArbiter(ska::cp::Arbiter& arb, ska::EntityId* entityId, unsigned int blockSize) {
	const auto& pc = m_entityManager.getComponent<PositionComponent>(*entityId);
	const auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(*entityId);
	const auto centerEntityPosition = ska::PositionComponent::getCenterPosition(pc, hc);
	const auto centerEntityBlock = ska::Point<float>{
		centerEntityPosition.x / blockSize,
		centerEntityPosition.y / blockSize
	};

	auto[bodyA, bodyB] = arb.getBodies();
	auto uniqueBlockPoints = std::unordered_set<ska::Point<int>>{};
	
	const auto cpSet = arb.getContactPoints();
	ska::Point<int> normal;
	ska::Point<int> lastBlockDirection;
	for(auto i = 0u; i < cpSet.count; i++) {
		const auto [bodyBlockPos, bodyEntityPos, bodyNormal] = ExtractBlockAndEntityFromContact(m_space, cpSet, bodyA, i);

		if (i == 0) {
			normal = bodyNormal;
		}

		const auto blockPoint = ska::Point<int>{
			static_cast<int>(bodyBlockPos.x / blockSize),
			static_cast<int>(bodyBlockPos.y / blockSize)
		};
		
		const auto calculatedBlockOffset = ska::Point<int>{ 
			static_cast<int>(static_cast<int>(bodyBlockPos.x) % blockSize),
			static_cast<int>(static_cast<int>(bodyBlockPos.y) % blockSize) };

		/*
		SKA_STATIC_LOG_DEBUG(ska::cp::SpaceCollisionEventSender)("Blocks float positions : block ",
			bodyBlockPos.x / blockSize, "; ", bodyBlockPos.y / blockSize,
			" integer : ",
			blockPoint.x, "; ", blockPoint.y,
			" offset (", calculatedBlockOffset.x, "; ", calculatedBlockOffset.y, ")");
		*/

		uniqueBlockPoints.insert(blockPoint);
		const auto variation = (ska::Point<int>(centerEntityBlock) - blockPoint) * normal;
		if (variation == 0) {
			lastBlockDirection = normal;
			uniqueBlockPoints.insert(blockPoint - normal);
		} else {
			lastBlockDirection = variation * normal;
			uniqueBlockPoints.insert(centerEntityBlock);
		}
	}

	auto wcc = ska::WorldCollisionComponent{};
	for (const auto& blockPoint : uniqueBlockPoints) {
		wcc.blockContacts.push_back(blockPoint * blockSize);
	}
	wcc.lastBlockDirection = lastBlockDirection;
	wcc.normal = ska::Point<float>{
		static_cast<float>(normal.x),
		static_cast<float>(normal.y)
	};
	return wcc;
}

ska::cp::SpaceCollisionEventSender::SpaceCollisionEventSender(EntityManager& em, Space& space, GameEventDispatcher& ged, unsigned blockSize) :
	m_space(space),
	m_entityManager(em) {
	m_callbackIndex = m_space.addDefaultCollisionCallback<CollisionHandlerType::BEGIN>([&, blockSize](ska::cp::Arbiter& arb, ska::cp::Space& space, ska::EntityId* entityId) {
		auto[bodyA, bodyB] = arb.getBodies();
		const auto isWorldCollision = bodyA == &space.getStaticBody() || bodyB == &space.getStaticBody();

		if (entityId != nullptr && isWorldCollision) {
			auto wcc = createWorldCollisionFromArbiter(arb, entityId, blockSize);

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
