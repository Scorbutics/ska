#pragma once
#include "Logger.h"

namespace ska {
	
	class LoggerClassFormatterTypeId {
	public:
		static std::string format(const char* rawClassName);
	};
	
	template <class T>
    class LoggerClassFormatter {
	public:
		static std::string format() {
			const auto rawClassName = typeid(T).name();
			return LoggerClassFormatterTypeId::format(rawClassName);
        }
	};
}
