#include "WalkAnimationStateMachine.h"
#include "Core/ECS/EntityManager.h"
#include "../GraphicComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"

ska::WalkAnimationStateMachine::WalkAnimationStateMachine(EntityManager& em) :
    AnimationStateMachine(),
	m_entityManager(em) {
}

void ska::WalkAnimationStateMachine::onEnter(const EntityId& e) {
	auto& gc = m_entityManager.getComponent<ska::GraphicComponent>(e);
	if (gc.animatedSprites.empty()) {
		return;
	}
	gc.animatedSprites[0].setOffset(Point<int>(0, 0));
}

void ska::WalkAnimationStateMachine::update(ska::AnimationComponent& ac, const EntityId& entityId) {
	auto& gc = m_entityManager.getComponent<GraphicComponent>(entityId);

	if (gc.animatedSprites.empty()) {
		return;
	}

	const auto& mov = m_entityManager.getComponent<MovementComponent>(entityId);

	//ska::Rectangle base de l'animation
	auto& texture = gc.animatedSprites[0];
	auto spritePos = texture.getOffsetBase();

	const auto xMove = ska::NumberUtils::round(mov.vx - mov.ax);
	const auto yMove = ska::NumberUtils::round(mov.vy - mov.ay);

	if (xMove == 0 && yMove == 0) {
		texture.stop(true);
		texture.reset();
	} else {
		texture.stop(false);
	}

	if (xMove != 0 || yMove != 0) {
		ac.state = RectangleUtils::getDirectionFromPos(Point<int>(0, 0), Point<int>(xMove, yMove));
		const auto& delay = 700 / ska::NumberUtils::squareroot(xMove * xMove + yMove * yMove);
		texture.setDelay(delay > 200 ? 200 : static_cast<unsigned int>(delay));		
	}

	const auto spriteHeight = texture.getHeight();
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