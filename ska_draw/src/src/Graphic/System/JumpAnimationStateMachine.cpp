#include "Graphic/__internalConfig/LoggerConfig.h"
#include "JumpAnimationStateMachine.h"
#include "Core/ECS/EntityManager.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "../GraphicComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"

ska::JumpAnimationStateMachine::JumpAnimationStateMachine(EntityManager& em) :
	m_entityManager(em) {
}

void ska::JumpAnimationStateMachine::update(ska::AnimationComponent& ac, const EntityId& entityId) {
	auto& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
	auto& mov = m_entityManager.getComponent<MovementComponent>(entityId);

	if (gc.animatedSprites.empty()) {
		return;
	}

	//ska::Rectangle base de l'animation
	auto& texture = gc.animatedSprites[0];
	auto spritePos = texture.getOffsetBase();
	const int spriteHeight = texture.getHeight();

	const auto xMove = NumberUtils::round(mov.vx);
	const auto yMove = NumberUtils::round(mov.vy);

	if (xMove != 0 || yMove != 0) {
		ac.state = RectangleUtils::getDirectionFromPos(Point<int>(0, 0), Point<int>(xMove, yMove));
	}

	switch (ac.state) {
	case 0:
		spritePos.y = spriteHeight * 2;
		break;
	case 1:
		spritePos.y = spriteHeight;
		break;
	case 2:
		spritePos.y = 0;
		break;
	case 3:
		spritePos.y = spriteHeight * 3;
		break;
	case 4:
		spritePos.y = spriteHeight * 7;
		break;
	case 5:
		spritePos.y = spriteHeight * 5;
		break;
	case 6:
		spritePos.y = spriteHeight * 4;
		break;
	case 7:
		spritePos.y = spriteHeight * 6;
		break;
	default:
		break;
	}
	texture.setOffset(spritePos);
}

void ska::JumpAnimationStateMachine::onEnter(const EntityId& e) {
	auto& gc = m_entityManager.getComponent<ska::GraphicComponent>(e);
	gc.animatedSprites[0].setOffset(Point<int>(3 * 48, 0));
	gc.animatedSprites[0].stop(true);
	gc.animatedSprites[0].switchToFrame(0);
}

