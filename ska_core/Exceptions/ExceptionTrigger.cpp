#include "ExceptionTrigger.h"
#include <functional>

const std::function<void(const ska::GenericException&)> ska::ExceptionAbort = [] (const GenericException& ge) {
	SKA_STATIC_LOG_ERROR(ska::GenericException)(std::string(ge.what()));
	exit(-1);
};
