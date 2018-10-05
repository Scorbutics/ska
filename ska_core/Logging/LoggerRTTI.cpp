#include <string>
#include "LoggerRTTI.h"

#ifdef __GNUC__
#ifdef __MINGW32__
	#include <cxxabi.h>
#endif
#endif

#ifdef __clang__
	#include <cxxabi.h>
#endif

std::string ska::LoggerClassFormatterTypeId::format(const char* rawClassName) {
	#ifdef __GNUC__
		#ifdef __MINGW32__
			int status;
			return abi::__cxa_demangle(rawClassName, 0, 0, &status);
		#endif
		return rawClassName;
	#elif defined(_MSC_VER)
		return rawClassName;
	#elif defined(__clang__)
		int status;
		return abi::__cxa_demangle(rawClassName, 0, 0, &status);
	#endif
	
	return rawClassName;
}