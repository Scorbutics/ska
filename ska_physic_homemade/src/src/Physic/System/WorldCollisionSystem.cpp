#include "WorldCollisionSystem.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/CollisionContact.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "Utils/RectangleUtils.h"
#include "World/CollisionProfile.h"

ska::WorldCollisionSystem::WorldCollisionSystem(EntityManager& entityManager, const CollisionProfile& cp, GameEventDispatcher& ged) :
	System(entityManager),
	m_collisionProfile(cp),
	m_ged(ged) {
}

ska::Rectangle ska::WorldCollisionSystem::calculateOverlap(Rectangle nextPos, const std::vector<Rectangle>& points) {
	const auto blockSize = m_collisionProfile.getBlockSize();

	if(points.empty()) {
		return Rectangle{0, 0, 0, 0};
	}

	Rectangle summedBlocksToOverlap;
	auto first = true;
	for (auto& p : points) {
		const auto currentRect = Rectangle{ p.x, p.y, static_cast<int>(blockSize), static_cast<int>(blockSize) };
		if (first) {
			summedBlocksToOverlap = currentRect;
			first = false;
		} else {
			summedBlocksToOverlap = RectangleUtils::unionRect(summedBlocksToOverlap, currentRect);
		}
	}

	return RectangleUtils::intersect(nextPos, summedBlocksToOverlap);
}
namespace ska {
	template <class T>
	void EraseAll(std::vector<T>& altered, const std::vector<T>& toErase) {
		using std::begin;
		using std::end;
		using std::remove_if;
		using std::find;

		altered.erase(remove_if(begin(altered), end(altered),
			[&](auto x) {return find(begin(toErase), end(toErase), x) != end(toErase); }), end(altered));
	}


	void FindAndEraseDoublons(std::vector<ska::Rectangle>& outputX, std::vector<ska::Rectangle>& outputY) {
		std::vector<ska::Rectangle> doublons;
		if (outputX.size() > outputY.size()) {
			EraseAll(outputY, outputX);
		}
		else {
			EraseAll(outputX, outputY);
		}

	}

	bool IntersectBlocksAtPos(const std::size_t layerMax, const ska::CollisionProfile& world, const ska::Rectangle& hitbox, std::vector<ska::Rectangle>& outputX, std::vector<ska::Rectangle>& outputY) {
		auto horizontalSegment = ska::Point<int>{ hitbox.x, hitbox.x + hitbox.w };
		auto verticalSegment = ska::Point<int>{ hitbox.y, hitbox.y + hitbox.h };
		const auto m_blockSize = world.getBlockSize();
		horizontalSegment /= m_blockSize;
		verticalSegment /= m_blockSize;

		auto col = false;

		auto intermediateX = std::vector<ska::Rectangle>{};
		auto intermediateY = std::vector<ska::Rectangle>{};

		for (auto x = horizontalSegment.x; x <= horizontalSegment.y; x++) {
			for (auto y = verticalSegment.x; y <= verticalSegment.y; y++) {
				if (world.collide(layerMax, x, y)) {
					const ska::Rectangle hitboxBlock{ static_cast<int>(x * m_blockSize), static_cast<int>(y * m_blockSize), static_cast<int>(m_blockSize), static_cast<int>(m_blockSize) };

					//Vertical
					const auto collisionContact = ska::CollisionContact{ hitbox, hitboxBlock };
					if (collisionContact.normal().y != 0) {
						SKA_STATIC_LOG_INFO(ska::WorldCollisionSystem)("Normal Y, overlap ", collisionContact.overlap().x, " ; ", collisionContact.overlap().y, " ; ", collisionContact.overlap().w, " ; ", collisionContact.overlap().h);
						outputY.push_back(hitboxBlock);
						col = true;
					}

					//Horizontal
					if (collisionContact.normal().x != 0) {
						SKA_STATIC_LOG_INFO(ska::WorldCollisionSystem)("Normal X, overlap ", collisionContact.overlap().x, " ; ", collisionContact.overlap().y, " ; ", collisionContact.overlap().w, " ; ", collisionContact.overlap().h);
						outputX.push_back(hitboxBlock);
						col = true;
					}
				}
			}
		}

		FindAndEraseDoublons(outputX, outputY);

		SKA_DEBUG_ONLY(
			if (!outputX.empty()) {
				SKA_STATIC_LOG_INFO(ska::WorldCollisionSystem)("X blocks : ", outputX.size());
			}

		if (!outputY.empty()) {
			SKA_STATIC_LOG_INFO(ska::WorldCollisionSystem)("Y blocks : ", outputY.size());
		}

		if (col) {
			SKA_STATIC_LOG_INFO(ska::WorldCollisionSystem)("Hitbox : (", hitbox.x, " ; ", hitbox.y, " ; ", hitbox.w, " ; ", hitbox.h, ")");
		}
		);

		return col;
	}
}

void ska::WorldCollisionSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		const auto& cc = m_componentAccessor.get<CollidableComponent>(entityId);
		if (cc.ghost) {
			continue;
		}

		/*
		 TODO ne pas utiliser l'ECS ici pour simplifier les dialogues par évènement avec le moteur

		 WorldCollisionComponent::BlockCollisionContainer lastBlockColPosX;
		WorldCollisionComponent::BlockCollisionContainer lastBlockColPosY;
		const auto& wccPtr = m_componentPossibleAccessor.get<WorldCollisionComponent>(entityId);
		if (wccPtr != nullptr) {
			const auto& wcc = *wccPtr;
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			//SKA_LOG_DEBUG("Suppression collision monde ", entityId);
			m_componentAccessor.remove<WorldCollisionComponent>(entityId);
			
			//Il faut rafraîchir l'ensemble des systèmes directement ici (au travers de l'EntityManager),
			//car il est possible de créer un autre WorldCollisionComponent dans le même tour de boucle.
			//Si le refresh n'est pas fait, il sera supprimé par le prochain rafraîchissement de l'EntityManager
			refreshEntity(entityId);
		}

		const auto hitbox = createHitBox(entityId, false, true);

		WorldCollisionComponent wcol;
		auto collided = false;

		wcol.blockColPosX.clear();
		wcol.blockColPosY.clear();
		const auto& intersect = IntersectBlocksAtPos(m_collisionProfile, hitbox, wcol.blockColPosX, wcol.blockColPosY);
		if (intersect) {
			wcol.xaxis = !wcol.blockColPosX.empty();
			wcol.yaxis = !wcol.blockColPosY.empty();
			const auto& overlapX = calculateOverlap(hitbox, wcol.blockColPosX);
			const auto& overlapY = calculateOverlap(hitbox, wcol.blockColPosY);
			SKA_LOG_MESSAGE("Overlap x summed : \t", overlapX.x, ";", overlapX.y, ";", overlapX.w, ";", overlapX.h);
			SKA_LOG_MESSAGE("Overlap y summed : \t", overlapY.x, ";", overlapY.y, ";", overlapY.w, ";", overlapY.h);
			wcol.contactX = CollisionContact{ overlapX, hitbox, overlapX };
			wcol.contactY = CollisionContact{ overlapY, hitbox, overlapY };
			if(wcol.contactY.hasCollision() || wcol.contactX.hasCollision()) {
				collided = true;
			}
		}

		if (collided) {
			CollisionEvent ce(entityId, &wcol, nullptr, m_componentAccessor.get<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}*/

	}
}

ska::Rectangle ska::WorldCollisionSystem::createHitBox(EntityId entityId, bool , bool ) const{
	auto& positionComponent = m_componentAccessor.get<PositionComponent>(entityId);
	auto& hitboxComponent = m_componentAccessor.get<HitboxComponent>(entityId);

	Rectangle hitBox;
	hitBox.x = NumberUtils::round(positionComponent.x + hitboxComponent.xOffset);
	hitBox.y = NumberUtils::round(positionComponent.y + hitboxComponent.yOffset);
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

