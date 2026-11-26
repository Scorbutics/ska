#pragma once

#include <sstream>
#include "SerializerValidator.h"
#include "SerializerOutputData.h"
#include "SerializerSafeZone.h"

namespace ska {

	class SerializerOutput : public SerializerValidator {
	public:
		SerializerOutput(SerializerOutputData data) :
			m_data(std::move(data)) {
		}

		template <std::size_t bytes>
		SerializerSafeZone<bytes> acquireMemory(std::string zoneName) {
			validateOrThrow();
			return SerializerSafeZone<bytes>{nullptr, *this, std::move(zoneName), m_data};
		}

		detail::SerializerSafeZone acquireMemory(std::size_t bytes, std::string zoneName) {
			validateOrThrow();
			return {nullptr, * this, std::move(zoneName), m_data, bytes};
		}

		~SerializerOutput() {
			validateOrAbort();
		}

	private:
		SerializerOutputData m_data;
	};

}
