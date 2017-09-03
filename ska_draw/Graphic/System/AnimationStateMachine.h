#pragma once
#include <functional>
#include "Utils/TupleUtils.h"
#include "ECS/Basics/Graphic/IAnimationStateMachine.h"
#include "ECS/ECSDefines.h"

namespace ska {
	class AnimationComponent;

	template <class ... S>
	class AnimationStateMachine :
		public IAnimationStateMachine {

		using Predicate = std::function<bool(EntityId&)>;

		template <class ASM>
		struct MetaDataAnimationStateMachine {
			MetaDataAnimationStateMachine() :
				nextASM(nullptr),
				defined(false) {
			}

			void defineCondition(Predicate&& cond) {
				condition = std::move(cond);
				defined = true;
			}

			using asmType = ASM;

			bool check(EntityId& e) const {
				return defined && condition(e);
			}

			IAnimationStateMachine* nextASM;

		private:
			Predicate condition;
			bool defined;

		};

	public:
		template<class T>
		void to(IAnimationStateMachine* toASM, Predicate&& cond1) {
			auto& l = std::get<MetaDataAnimationStateMachine<T>>(m_linkedTo);
			l.defineCondition(std::forward<Predicate>(cond1));
			l.nextASM = toASM;
		}

		IAnimationStateMachine* animate(AnimationComponent& ac, EntityId& e) override {
			IAnimationStateMachine* next = nullptr;
			auto index = 0;
			update(ac, e);
			meta::for_each_in_tuple(m_linkedTo, [&index, &next, &e](const auto& mdASM) {
				if (mdASM.check(e)) {
					next = mdASM.nextASM;
				}
				index++;
			});
			return next;
		}

		virtual ~AnimationStateMachine() = default;

	protected:
		virtual void update(AnimationComponent& ac, EntityId& e) = 0;

	private:
		std::tuple<MetaDataAnimationStateMachine<S>...> m_linkedTo;
	};
}
