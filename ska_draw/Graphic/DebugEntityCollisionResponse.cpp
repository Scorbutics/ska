#include "Data/Events/CollisionEvent.h"
#include "DebugEntityCollisionResponse.h"
#include "ECS/EntityManager.h"
#include "Utils/SpritePath.h"
#include "ECS/Basics/Graphic/DeleterComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "GraphicComponent.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "Core/CodeDebug/CodeDebug.h"

ska::DebugEntityCollisionResponse::DebugEntityCollisionResponse(GameEventDispatcher& ged, EntityManager& em) :
EntityCollisionObserver(std::bind(&DebugEntityCollisionResponse::onEntityCollision, this, std::placeholders::_1), ged),
	m_entityManager(em),
	m_ged(ged) {
}

bool ska::DebugEntityCollisionResponse::onEntityCollision(CollisionEvent& e) const{
	if (e.collisionComponent != nullptr) {
		auto& col = *e.collisionComponent;
		auto entity = m_entityManager.createEntity();
		GraphicComponent gc;
		gc.sprites.resize(1);
		gc.sprites[0].loadFromColoredRect(col.overlap.w, col.overlap.h, ska::Color(255, 0, 0, 4));
		m_entityManager.addComponent<GraphicComponent>(entity, std::move(gc));
		DeleterComponent dc;
		dc.delay = 2000;
		m_entityManager.addComponent<DeleterComponent>(entity, std::move(dc));
		PositionComponent pc(ska::Point<float>(col.overlap.x, col.overlap.y));
		m_entityManager.addComponent<PositionComponent>(entity, std::move(pc));

		/*SKA_DBG_ONLY(
			if (col.xaxis && col.yaxis) {
				const auto& intersection = col.overlap;
				const auto& hcTarget = m_entityManager.getComponent<HitboxComponent>(col.target);
				const auto& hcOrigin = m_entityManager.getComponent<HitboxComponent>(col.origin);
				SKA_LOG_INFO("Collision deux axes ", intersection.w, "/", (hcOrigin.width > hcTarget.width ? hcTarget.width : hcOrigin.width), ";", intersection.h, "/", (hcOrigin.height > hcTarget.height ? hcTarget.height : hcOrigin.height));
			}
			else {
				const auto& intersection = col.overlap;
				const auto& hcTarget = m_entityManager.getComponent<HitboxComponent>(col.target);
				const auto& hcOrigin = m_entityManager.getComponent<HitboxComponent>(col.origin);
				SKA_LOG_INFO("Collision ", intersection.w, "/", (hcOrigin.width > hcTarget.width ? hcTarget.width : hcOrigin.width), ";", intersection.h, "/", (hcOrigin.height > hcTarget.height ? hcTarget.height : hcOrigin.height));
			}
		);*/
	}
	return true;
}
