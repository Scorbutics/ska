#pragma once
#include "Logger.h"

namespace ska {
	template<class T>
    class LoggerClassFormatter {
	public:
		static std::string format() {
			return typeid(T).name();
		}
	};
}