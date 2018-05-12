#pragma once
#include <string>
#include <exception>

namespace ska {
	class GenericException : public std::exception {
	public:
		GenericException(const char*  message = "Error", const char* type = nullptr);
		const char* what() const noexcept override;
		~GenericException() = default;
	
	private:
		std::string m_message;
		std::string m_typedMessage;
	};

}
