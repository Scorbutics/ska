#include "IADefinedMovementSystem.h"
#include "Utils/TimeUtils.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "Physic/Space.h"
#include "Physic/PredefinedCollisionTypes.h"

ska::IADefinedMovementSystem::IADefinedMovementSystem(cp::Space& space, EntityManager& entityManager, GameEventDispatcher& ged) : 
	System(entityManager), 
	m_space(space),
	m_eventDispatcher(ged) {
}

void ska::IADefinedMovementSystem::refresh(unsigned int) {
	std::vector<EntityId> entityWithComponentsToDelete;

	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& fc = m_componentAccessor.get<ForceComponent>(entityId);
		auto& mc = m_componentAccessor.get<MovementComponent>(entityId);
		auto& pc = m_componentAccessor.get<PositionComponent>(entityId);
		auto& iamc = m_componentAccessor.get<IADefinedMovementComponent>(entityId);
		auto& cc = m_componentAccessor.get<CollidableComponent>(entityId);
		const auto& hcChipmunk = m_componentAccessor.get<ska::cp::HitboxComponent>(entityId);
		const auto& hc = m_componentAccessor.get<HitboxComponent>(entityId);
		const auto& centerPos = PositionComponent::getCenterPosition(pc, hc);

		if (iamc.directionIndex >= iamc.directions.size()) {
			continue;
		}

		const auto& targetVector = iamc.directions[iamc.directionIndex];
		auto polarVectorMovement = PolarPoint<float>::polar(targetVector.x, targetVector.y);

		/* speed */
		polarVectorMovement.radius = iamc.speed;

		const auto targetPoint = targetVector + iamc.origin;
		const auto finalMovement = Point<float>::cartesian(polarVectorMovement.radius, polarVectorMovement.angle);

		/* Either the time is up, or the goal is reached (if we are going farer and farer from the target pos, goal is reached) */
		const auto distanceSquaredToTarget = RectangleUtils::distanceSquared<float>(centerPos, targetPoint);
		const auto nextDistanceSquaredToTarget = RectangleUtils::distanceSquared<float>(centerPos + finalMovement, targetPoint);
		const auto directionChanged = distanceSquaredToTarget < nextDistanceSquaredToTarget;

		bool collisioned;
		if (cc.ghost) {
			m_componentAccessor.remove<WorldCollisionComponent>(entityId);
			collisioned = false;
		} else {
			collisioned = m_componentPossibleAccessor.get<WorldCollisionComponent>(entityId) != nullptr;
			
			SKA_DEBUG_ONLY(if (collisioned) {
				SKA_LOG_DEBUG("World collision ", collisioned);
			});
		}
		auto finished = false;
		if (TimeUtils::getTicks() - iamc.lastTimeStarted >= iamc.delay || directionChanged || collisioned) {

			iamc.origin = iamc.directions[iamc.directionIndex];
			if (iamc.directionIndex+1 < iamc.directions.size()) {
				iamc.directionIndex++;
			} else if (iamc.loop) {
				iamc.directionIndex = 0;
			} else {
				finished = true;
				if (iamc.callbackActive) {
					/* triggers callback */
					const auto scriptEntity = createEntity();
					auto events = std::vector<ScriptEventType>();
					events.push_back(ScriptEventType::ScriptCreate);
					auto se = ScriptEvent{ std::move(events), std::move(iamc.callback), entityId, {}, {}, {}, pc };
					m_eventDispatcher.Observable<ScriptEvent>::notifyObservers(se);
				}
				/* removes component */
				entityWithComponentsToDelete.push_back(entityId);
			}

			iamc.lastTimeStarted = TimeUtils::getTicks();
		}

		if (!finished) {
			SKA_LOG_DEBUG("IA Movement (", finalMovement.x, " : ", finalMovement.y, ")");
			fc.x = finalMovement.x;
			fc.y = finalMovement.y;
			//TODO dans un autre système ?
			auto& entityShape = m_space.getShape(hcChipmunk.shapeIndex);
			entityShape.setFilter(CP_SHAPE_FILTER_NONE.group, CP_SHAPE_FILTER_NONE.categories, CP_SHAPE_FILTER_NONE.mask);
			mc.ay = 0;
			mc.ax = 0;
		} else if (directionChanged) {
			fc.x = 0;
			fc.y = 0;
			mc.ay = 0;
			mc.ax = 0;
		} 
		
		if (finished) {
			//TODO dans un autre système ?
			auto& entityShape = m_space.getShape(hcChipmunk.shapeIndex);
			entityShape.setFilter(CP_SHAPE_FILTER_ALL.group, CP_SHAPE_FILTER_ALL.categories, CP_SHAPE_FILTER_ALL.mask);
			cc.ghost = false;
		}
	}

	for (auto& id : entityWithComponentsToDelete) {
		m_componentAccessor.remove<IADefinedMovementComponent>(id);
	}
}

ska::IADefinedMovementSystem::~IADefinedMovementSystem() {
}
