#pragma once
#include <vector>
#include <memory>
#include "../Module.h"

namespace ska {
	class GameConfiguration {
		using ModulePtr = std::unique_ptr<Module>;

	public:
		GameConfiguration() = default;
		~GameConfiguration() = default;

		template <class Module>
		void requireModule(const std::string& moduleName) {
			static_assert(std::is_base_of<ska::Module, Module>::value, "The module to load must inherit from Module");
			//m_modules.push_back(std::move(ModulePtr(new Module(moduleName))));
		}

	private:
		std::vector<ModulePtr> m_modules;
	};
}
