#pragma once
#include <unordered_set>
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/System.h"
#include "../HasShadowComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "AbstractGraphicSystem.h"
#include "../AnimatedTexture.h"

namespace ska {
    class ShadowSystem :
        public AbstractGraphicSystem,
        public System< RequiredComponent<PositionComponent, HitboxComponent, HasShadowComponent>, PossibleComponent<>> {
    public:
		ShadowSystem(EntityManager& entityManager, CameraSystem& camera);
        ShadowSystem& operator=(const ShadowSystem&) = delete;
        ~ShadowSystem() override = default;
        virtual void update(unsigned int ellapsedTime) override;
    protected:
        virtual void refresh(unsigned int ellapsedTime) override;

    private:
        AnimatedTexture m_shadow;
    };
}
