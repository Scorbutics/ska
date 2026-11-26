#include <iostream>
#include "SerializerValidator.h"

bool ska::SerializerValidator::ShouldAbort = true;

void ska::SerializerValidator::DisableAbort() {
    ShouldAbort = false;
}

void ska::SerializerValidator::onError(SerializerErrorData data) {
    m_errors.push_back(std::move(data));
}

void ska::SerializerValidator::validateOrThrow() {
    validate([](std::vector<SerializerErrorData> errors) {
        throw SerializationMemoryException(std::move(errors));
    });
}

void ska::SerializerValidator::validateOrAbort() noexcept {
    try {
        validateOrThrow();
    } catch (const SerializationMemoryException& sme) {
        std::cerr << sme.what() << std::endl;
        
        for (const auto& errorData : sme.data) {
            std::cerr << "in zone \"" << errorData.zone << "\", ";
            std::cerr << errorData.bytesAllocated << " bytes allocated / ";
            std::cerr << errorData.bytesAvailable << " total available bytes, and ";
            std::cerr << errorData.bytesRequested << " are now requested." << std::endl;
            std::cerr << sme.what() << std::endl;
        }
        
        if (SerializerValidator::ShouldAbort) {
            abort();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        if (SerializerValidator::ShouldAbort) {
            abort();
        }
    } catch (...) {
        std::cerr << "Unknown error occured while serialization process" << std::endl;
        if (SerializerValidator::ShouldAbort) {
            abort();
        }
    }
}
