#include "Graphic/__internalConfig/LoggerConfig.h"
#include "RunAnimationStateMachine.h"
#include "CharsetRows.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "../GraphicComponent.h"

ska::RunAnimationStateMachine::RunAnimationStateMachine(EntityManager& em, unsigned int firstColumn) :
	m_entityManager(em),
	m_firstColumn(firstColumn) {
}

void ska::RunAnimationStateMachine::onEnter(const EntityId& e) {
	auto& gc = m_entityManager.getComponent<GraphicComponent>(e);
	if (gc.animatedSprites.empty()) {
		return;
	}
	auto& texture = gc.animatedSprites[0];
	texture.setOffset(Point<int>(static_cast<int>(m_firstColumn * texture.getWidth()), 0));
	// Its first stride at once
	texture.switchToFrame(0);
	m_entityManager.getComponent<AnimationComponent>(e).walkShown = true;
}

void ska::RunAnimationStateMachine::update(AnimationComponent& ac, const EntityId& entityId) {
	auto& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
	if (gc.animatedSprites.empty()) {
		return;
	}
	auto& texture = gc.animatedSprites[0];
	if (ac.walking) {
		texture.setDelay(ac.walkDelay);
		texture.stop(false);
	} else {
		texture.stop(true);
		texture.reset();
	}
	auto spritePos = texture.getOffsetBase();
	const auto row = CharsetRow(ac.state);
	if (row >= 0) {
		spritePos.y = row * static_cast<int>(texture.getHeight());
	}
	texture.setOffset(spritePos);
}
