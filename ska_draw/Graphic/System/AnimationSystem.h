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
      using Predicate = std::function<bool()>;

      template <class T>
      using ASMPtr = std::unique_ptr<T>;

    protected:
      template<class ASM1, class ... Args>
      void set(Args&&... args) {
          using animationStateMachineTypeFound = ska::meta::contains<ASM1, ASM...>;
          static_assert(animationStateMachineTypeFound::value, "Unable to locate animation state machine in the current animation system");
          std::get<ASMPtr<ASM1>>(m_asm) = std::make_unique<ASM1>(std::forward<Args>(args)...);
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

         //example
         /*static bool done = false;
         if(!done) {
                for(auto& c : m_components) {
                    c.animationFiniteStateMachine = asm1.get();
                    c.animationFiniteStateMachine->onEnter();
                }
         }*/
         //end

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
          IAnimationStateMachine* next = c.animationFiniteStateMachine->animate(c, m_entity);
          if(next != nullptr) {
              c.animationFiniteStateMachine = next;
              c.animationFiniteStateMachine->onEnter();
          }
        }
      }

    private:
      std::tuple<ASMPtr<ASM>...> m_asm;

    };

}

