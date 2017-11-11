#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "ECS/Basics/Graphic/AnimationComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/System.h"
#include "AnimationStateMachine.h"

namespace ska {
    template <class ... ASM>
    class AnimationSystem :
        public System<std::unordered_set<EntityId>, RequiredComponent<GraphicComponent, MovementComponent, AnimationComponent>, PossibleComponent<PositionComponent>> {
      using Predicate = std::function<bool(EntityId&)>;

      template <class T>
      using ASMPtr = std::unique_ptr<T>;

    public:
      template<class ASM1, class ... Args>
	  ASMPtr<ASM1>& setup(Args&&... args) {
          using animationStateMachineTypeFound = ska::meta::contains<ASM1, ASM...>;
          static_assert(animationStateMachineTypeFound::value, "Unable to locate animation state machine in the current animation system");
		  auto& toSet = std::get<ASMPtr<ASM1>>(m_asm);
      	  toSet = std::make_unique<ASM1>(std::forward<Args>(args)...);
		  return toSet;
      }


      template<class ASM1, class ASM2>
      void link(Predicate&& cond) {
          using animationStateMachineTypeFound1 = ska::meta::contains<ASM1, ASM...>;
          using animationStateMachineTypeFound2 = ska::meta::contains<ASM2, ASM...>;
          static_assert(animationStateMachineTypeFound1::value && animationStateMachineTypeFound2::value,
                        "Unable to locate animation state machine in the current animation system");

            auto& asm1 = std::get<std::unique_ptr<ASM1>>(m_asm);
            auto& asm2 = std::get<std::unique_ptr<ASM2>>(m_asm);
            if(asm1 == nullptr || asm2 == nullptr) {
                throw std::runtime_error("asm1 or asm2 is not allocated");
            }

        asm1->template to<ASM2>(asm2.get(), std::forward<Predicate>(cond));
      }

    public:
        explicit AnimationSystem(EntityManager& entityManager) :
            System(entityManager) {
        }
        AnimationSystem& operator=(const AnimationSystem&) = delete;
        virtual ~AnimationSystem() = default;

      virtual void refresh(unsigned int ellapsedTime) override {
        const auto& processed = getEntities();
        for (auto entityId : processed) {
          auto& c = m_componentAccessor.get<ska::AnimationComponent>(entityId);
		  auto afsm = c.getASM();
	      auto next = afsm != nullptr ? afsm->animate(c, entityId) : nullptr;
          if(next != nullptr) {
              c.setASM(*next, entityId);
          }
        }
      }

    private:
      std::tuple<ASMPtr<ASM>...> m_asm;

    };

}

