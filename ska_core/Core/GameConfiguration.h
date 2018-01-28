#pragma once
#include <memory>
#include "../Utils/TupleUtils.h"
#include "../Utils/ContainsTypeTuple.h"
#include "../Utils/unsmartptr.h"
#include <cassert>

namespace ska {
	class StateHolder;

    template <class EventDispatcher, class ... Modules>
	class GameConfiguration {
		template <class T>
		using ModulePtr = std::unique_ptr<T>;
		using ModuleTuple = std::tuple<ModulePtr<Modules>...>;

	public:
		GameConfiguration() = default;
		GameConfiguration(const GameConfiguration&) = delete;
		GameConfiguration(GameConfiguration&&) = default;
		~GameConfiguration() = default;

		template <class Module, class ... Args>
		Module& requireModule(const std::string& moduleName, Args&& ... args) {
			static_assert(meta::contains<Module, Modules...>::value, "The module to load must belong to the provided Module list");
			auto mod = std::make_unique<Module>(moduleName, std::forward<Args>(args)...);
			auto& moduleRef = static_cast<Module&>(*mod);
			std::get<ModulePtr<Module>>(m_modules) = std::move(mod);
			return moduleRef;
		}

		void eventUpdate(unsigned int ellapsedTime) {
			ska::meta::for_each_in_tuple(m_modules, [&ellapsedTime](auto& m) {
				using rawModuleType = typename meta::unsmart_ptr<decltype(m)>::type;
				if constexpr(has_eventUpdate<rawModuleType>::value) {
					assert(m != nullptr && "This module type is not initialized");
					m->eventUpdate(ellapsedTime);
				}
			});
		}

		void graphicUpdate(unsigned int ellapsedTime, StateHolder& sh) {
			ska::meta::for_each_in_tuple(m_modules, [&ellapsedTime, &sh](auto& m) {
				using rawModuleType = typename meta::unsmart_ptr<decltype(m)>::type;
				if constexpr(has_graphicUpdate<rawModuleType>::value) {
					assert(m != nullptr && "This module type is not initialized");
					m->graphicUpdate(ellapsedTime, sh);
				}
			});
		}

		EventDispatcher& getEventDispatcher() {
            return m_eventDispatcher;
		}
		
	private:

    	template<typename C>
		struct has_eventUpdate {
		private:

			template<typename T>
			static constexpr auto check(T*)
				-> typename
				std::is_same<
					decltype(std::declval<T>().eventUpdate(std::declval<unsigned int>())),
					void
				>::type;

			template<typename>
			static constexpr std::false_type check(...);

			typedef decltype(check<C>(0)) type;

		public:
			static constexpr bool value = type::value;
		};

		template<typename C>
		struct has_graphicUpdate {
		private:
			template<typename T>
			static constexpr auto check(T*)
				-> typename
				std::is_same<
					decltype(std::declval<T>().graphicUpdate(std::declval<unsigned int>(), std::declval<StateHolder&>())),
					void
				>::type;

			template<typename>
			static constexpr std::false_type check(...);

			typedef decltype(check<C>(0)) type;

		public:
			static constexpr bool value = type::value;
		};

		EventDispatcher m_eventDispatcher;
		ModuleTuple m_modules;
		
	};
}
