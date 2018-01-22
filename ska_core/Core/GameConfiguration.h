#pragma once
#include <vector>
#include <memory>
#include "../Module.h"

namespace ska {
	class GameConfiguration {
		using ModulePtr = std::unique_ptr<Module>;

	public:
		GameConfiguration() = default;
		GameConfiguration(GameConfiguration&&) = default;
		~GameConfiguration() = default;

		template <class Module, class ... Args>
		void requireModule(const std::string& moduleName, Args&& ... args) {
			static_assert(std::is_base_of<ska::Module, Module>::value, "The module to load must inherit from Module");
			m_modules.push_back(ModulePtr(new Module(moduleName, std::forward<Args>(args)...)));
		}

		std::vector<ModulePtr>& getModules() {
		    return m_modules;
		}

	private:
		std::vector<ModulePtr> m_modules;
	};
}
