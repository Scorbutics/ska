#pragma once
#include <functional>
#include "../Utils/SkaConstants.h"
#include "GenericException.h"
#include "../Logging/Logger.h"

namespace ska {

	template<class Exception>
	void ExceptionTrigger(const std::string& message, std::function<void(const GenericException& message)> exceptionDisabledHandler) {
		#ifdef SKA_EXCEPTIONS_DISABLED
		exceptionDisabledHandler(Exception(message.c_str()));
		#else
		throw Exception(message.c_str());
		#endif
	}

	static const std::function<void(const GenericException& ge)> ExceptionAbort;
	
}
