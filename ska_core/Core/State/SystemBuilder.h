#pragma once
#include <algorithm>
#include "../Priorized.h"

namespace ska {
	/**
	 * \brief Internal helper class that builds states and sub-states, as a factory.
	 * \tparam EntityManager 	 
	 */
	template <class EntityManager>
	class SystemBuilder {
		public:
			explicit SystemBuilder(EntityManager& em) :
				m_entityManager(em) {
			}

			template<class System, class ...Args>
			std::unique_ptr<System> createLogic(Args&&... args) {
				return std::make_unique<System>(m_entityManager, std::forward<Args>(args)...);
			}

			template<class System, class ...Args>
			System* addPriorizedLogic(std::vector<std::unique_ptr<ISystem>>& logics, int priority, Args&& ... args) {
				auto s = std::move(createLogic<System, Args...>(std::forward<Args>(args)...));
				s->setPriority(priority);
				auto result = static_cast<System*>(s.get());
				logics.push_back(std::move(s));
				std::sort(logics.begin(), logics.end(), Priorized::comparatorInf<std::unique_ptr<ISystem>>);
				return result;
			}

			template<class System, class ...Args>
			System* addPriorizedGraphic(std::vector<std::unique_ptr<IGraphicSystem>>& graphics, int priority, Args&& ... args) {
				auto system = std::make_unique<System>(m_entityManager, std::forward<Args>(args)...);
				auto result = static_cast<System*>(system.get());
				graphics.push_back(std::move(system));
				std::sort(graphics.begin(), graphics.end(), Priorized::comparatorInf<std::unique_ptr<IGraphicSystem>>);
				result->Priorized::setPriority(priority);
				return result;
			}

	private:
		EntityManager& m_entityManager;
	};
}
