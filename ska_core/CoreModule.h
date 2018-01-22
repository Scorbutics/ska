#pragma once
#include "Module.h"
#include <string>

namespace ska {
	class CoreModule :
		public Module {
	public:
		explicit CoreModule(const std::string& moduleName);
		void eventUpdate(unsigned int ellapsedTime) override;
		~CoreModule();
	};
}
