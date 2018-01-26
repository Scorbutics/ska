#pragma once
#include <vector>
#include <memory>
#include "../Module.h"

namespace ska {

    template <class EventDispatcher>
	class GameConfiguration {
		using ModulePtr = std::unique_ptr<Module>;

	public:
		GameConfiguration() = default;
		GameConfiguration(const GameConfiguration&) = delete;
		GameConfiguration(GameConfiguration&&) = default;
		~GameConfiguration() = default;

		template <class Module, class ... Args>
		Module& requireModule(const std::string& moduleName, Args&& ... args) {
			static_assert(std::is_base_of<ska::Module, Module>::value, "The module to load must inherit from Module");
			auto mod = ModulePtr(new Module(moduleName, std::forward<Args>(args)...));
			auto& moduleRef = static_cast<Module&>(*mod);
			m_modules.push_back(std::move(mod));
			return moduleRef;
		}

		std::vector<ModulePtr>& getModules() {
		    return m_modules;
		}

		EventDispatcher& getEventDispatcher() {
            return m_eventDispatcher;
		}

	private:
		std::vector<ModulePtr> m_modules;
		EventDispatcher m_eventDispatcher;
	};
}
