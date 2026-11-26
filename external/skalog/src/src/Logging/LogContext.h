#pragma once

#include <string>
#include "LogLevel.h"

namespace ska {
	namespace loggerdetail {
		struct LogContext {
			LogLevel logLevel = LogLevel::Debug;
			std::string className;

			//!! static memory data !!
			const char* function = nullptr;
			const char* file = nullptr;
			
			unsigned long line = 0;
		};
	}
}
