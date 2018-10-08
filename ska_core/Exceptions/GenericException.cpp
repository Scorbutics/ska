#include "GenericException.h"
#include "../Logging/LoggerRTTI.h"

ska::GenericException::GenericException(const char* message, const char* type) {
    static Logger<GenericException> logger;
    
    m_message = std::string(message);
	m_typedMessage = std::string((type == nullptr ? "" : (std::string(type) + " : ")) + m_message);
	logger.log<LogLevel::Warn>() << "Exception thrown : " <<  m_typedMessage;
}

const char* ska::GenericException::what() const noexcept {
	return m_typedMessage.c_str();
}



