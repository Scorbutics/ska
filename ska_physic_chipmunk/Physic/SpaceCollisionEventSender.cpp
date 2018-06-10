#include "SpaceCollisionEventSender.h"
#include "Utils/RectangleUtils.h"
#include "Utils/NumberUtils.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Data/Events/CollisionEvent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/EntityManager.h"
#include "Data/Events/GameEventDispatcher.h"
#include "PredefinedCollisionTypes.h"
#include "../CollisionHandlerType.h"

namespace ska {
	namespace cp {
		
		std::pair<cpVect, cpVect> ExtractBlockAndEntityFromContact(Space& space, const cpContactPointSet& cpSet, cpBody* body, const std::size_t index) {
			cpVect bodyBlock;
			cpVect bodyEntity;

			if (body == &space.getStaticBody()) {
				bodyBlock = cpSet.points[index].pointB;
				bodyEntity = cpSet.points[index].pointA;
			} else {
				bodyBlock = cpSet.points[index].pointA;
				bodyEntity = cpSet.points[index].pointB;
			}
			return std::make_pair(bodyBlock, bodyEntity);
		}
	}
}

ska::WorldCollisionComponent ska::cp::SpaceCollisionEventSender::createWorldCollisionFromArbiter(ska::cp::Arbiter& arb, ska::EntityId* entityId, unsigned int blockSize) {
	const auto& pc = m_entityManager.getComponent<PositionComponent>(*entityId);
	const auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(*entityId);
	const auto centerEntityPosition = ska::PositionComponent::getCenterPosition(pc, hc);
	const auto centerEntityBlock = ska::Point<int>{ centerEntityPosition / blockSize };

	auto[bodyA, bodyB] = arb.getBodies();
	const auto cpSet = arb.getContactPoints();

	auto uniqueBlockPoints = std::unordered_set<ska::Point<int>>{};

	ska::Point<int> normal;

	for(auto i = 0u; i < cpSet.count; i++) {
		auto[bodyBlockPos, bodyEntityPos] = ExtractBlockAndEntityFromContact(m_space, cpSet, bodyA, i);

		auto blockPoint = ska::Point<int>{
			static_cast<int>(bodyBlockPos.x / blockSize),
			static_cast<int>(bodyBlockPos.y / blockSize)
		};

		if (i == 0) {
			const auto sign = ska::Point<int>{
				bodyBlockPos.x < bodyEntityPos.x ? -1 : 1,
				bodyBlockPos.y < bodyEntityPos.y ? -1 : 1
			};

			normal = ska::Point<int>{				
				static_cast<int>(ska::NumberUtils::absolute(arb.getNormal().x) * sign.x),
				static_cast<int>(ska::NumberUtils::absolute(arb.getNormal().y) * sign.y)
			};
		}
		
		const auto calculatedBlockOffset = ska::Point<int>{ 
			static_cast<int>(static_cast<int>(bodyBlockPos.x) % blockSize),
			static_cast<int>(static_cast<int>(bodyBlockPos.y) % blockSize) };

		SKA_STATIC_LOG_DEBUG(ska::cp::SpaceCollisionEventSender)("Blocks float positions : block ",
			bodyBlockPos.x / blockSize, "; ", bodyBlockPos.y / blockSize,
			" integer : ",
			blockPoint.x, "; ", blockPoint.y,
			" offset (", calculatedBlockOffset.x, "; ", calculatedBlockOffset.y, ")");

		/*const auto calculatedEntityOffset = ska::Point<int>{
			static_cast<int>(static_cast<int>(bodyEntity.x) % blockSize),
			static_cast<int>(static_cast<int>(bodyEntity.y) % blockSize) };

		SKA_STATIC_LOG_DEBUG(ska::cp::SpaceCollisionEventSender)("Entity float positions : block ", bodyEntity.x / blockSize, "; ", bodyEntity.y / blockSize,
			" offset (", calculatedEntityOffset.x, "; ", calculatedEntityOffset.y, ")");*/

		//SKA_STATIC_LOG_DEBUG(ska::cp::SpaceCollisionEventSender)("Normal ", contactNormalAbs.x, "; ", contactNormalAbs.y, " sign ", sign.x, "; ", sign.y);
		

		uniqueBlockPoints.insert(blockPoint);
		if (centerEntityBlock == blockPoint) {
			uniqueBlockPoints.insert(blockPoint + normal);
		} else {
			uniqueBlockPoints.insert(blockPoint - normal);
		}
	}

	auto wcc = ska::WorldCollisionComponent{};
	for (const auto& blockPoint : uniqueBlockPoints) {
		wcc.blockContacts.push_back(blockPoint * blockSize);
	}

	wcc.normal = ska::Point<float>{
		static_cast<float>(-normal.x),
		static_cast<float>(-normal.y)
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
