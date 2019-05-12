#include "Graphic/__internalConfig/LoggerConfig.h"
#include "DebugCollisionDrawerSystem.h"
#include "Core/Utils/SpritePath.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../GraphicComponent.h"
#include "Core/ECS/Basics/Graphic/DeleterComponent.h"

ska::DebugCollisionDrawerSystem::DebugCollisionDrawerSystem(EntityManager& em) : System(em) {
}

void ska::DebugCollisionDrawerSystem::refresh(unsigned int) {
	std::vector<EntityId> removeComponents;

	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& dcgc = m_componentAccessor.get<DebugGraphicComponent>(entityId);
		if (((dcgc.typeMask & COLLISION) == COLLISION)) {

			if (dcgc.collidedBlocks.size() >= 10) {
				scheduleDeferredRemove(dcgc.collidedBlocks.front());
				dcgc.collidedBlocks.pop();
				scheduleDeferredRemove(dcgc.collidedBlocks.front());
				dcgc.collidedBlocks.pop();
			}
			
			for (const auto& p : dcgc.blockColPosX) {
				dcgc.collidedBlocks.push(createDebugCollisionEntity(p, 1));
			}			

			for (const auto& p : dcgc.blockColPosY) {
				dcgc.collidedBlocks.push(createDebugCollisionEntity(p, 1));
			}

		}
		if ((dcgc.typeMask & WALK) == WALK) {
			createDebugCollisionEntity(PositionComponent::getCenterPosition(m_componentAccessor.get<PositionComponent>(entityId), m_componentAccessor.get<HitboxComponent>(entityId)), 1);
		}
		removeComponents.push_back(entityId);
	}

	for (auto& e : removeComponents) {
		m_componentAccessor.remove<DebugGraphicComponent>(e);
	}
}

ska::EntityId ska::DebugCollisionDrawerSystem::createDebugCollisionEntity(const Point<int>& p, int offset) {
	EntityId e = createEntity();
	GraphicComponent gc;
	gc.sprites.resize(1);
	gc.sprites[0].loadFromColoredRect(48, 48, ska::Color(255, 0, 0, offset == 0 ? 40 : 100));
	gc.desiredPriority = std::numeric_limits<int>::max() - 1;
	m_componentAccessor.add<GraphicComponent>(e, std::move(gc));
	DeleterComponent dc;
	dc.delay = 3000;
	m_componentAccessor.add<DeleterComponent>(e, std::move(dc));
	PositionComponent pc = p;
	m_componentAccessor.add<PositionComponent>(e, std::move(pc));
	return e;
}
