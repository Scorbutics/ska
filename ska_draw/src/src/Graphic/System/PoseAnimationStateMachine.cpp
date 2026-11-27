#include "Graphic/__internalConfig/LoggerConfig.h"
#include "PoseAnimationStateMachine.h"
#include "CharsetRows.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "../GraphicComponent.h"

ska::PoseAnimationStateMachine::PoseAnimationStateMachine(EntityManager& em) :
	m_entityManager(em) {
}

void ska::PoseAnimationStateMachine::onEnter(const EntityId& e) {
	auto& gc = m_entityManager.getComponent<GraphicComponent>(e);
	if (gc.animatedSprites.empty()) {
		return;
	}
	auto& texture = gc.animatedSprites[0];
	texture.stop(true);
	texture.switchToFrame(0);
	m_entityManager.getComponent<AnimationComponent>(e).walkShown = false;
}

void ska::PoseAnimationStateMachine::update(AnimationComponent& ac, const EntityId& entityId) {
	auto& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
	if (gc.animatedSprites.empty() || ac.pose < 0) {
		return;
	}
	auto& texture = gc.animatedSprites[0];
	texture.stop(true);
	texture.switchToFrame(0);
	const auto row = CharsetRow(ac.state);
	texture.setOffset(Point<int>(ac.pose * static_cast<int>(texture.getWidth()), (row < 0 ? 0 : row) * static_cast<int>(texture.getHeight())));
}
