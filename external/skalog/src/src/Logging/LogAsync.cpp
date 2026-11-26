#include "LogAsync.h"
#include "LoggerImpl.h"

void ska::LogPayload::operator()() {
	if (entry != nullptr) {
		assert(logger != nullptr);
		logger->consumeNow(*entry);
	} else {
		lambda();
	}
}