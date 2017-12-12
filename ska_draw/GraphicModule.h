#pragma once
#include "Module.h"

namespace ska {
	class GraphicModule : 
		public ska::Module {
	public:
		explicit GraphicModule(const std::string& name);
		~GraphicModule();
	};
}
