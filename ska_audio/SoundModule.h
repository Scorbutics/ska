#pragma once
#include "Module.h"
#include <string>

namespace ska {
	class SoundModule : public ska::Module {
	public:
		explicit SoundModule(const std::string& moduleName);
		~SoundModule();
	};
}
