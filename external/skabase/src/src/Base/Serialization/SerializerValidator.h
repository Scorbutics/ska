#pragma once

#include <cstddef>
#include <sstream>
#include <vector>
#include "SerializerErrorData.h"
#include "SerializationMemoryException.h"

namespace ska {

	class SerializerValidator {
	public:
		SerializerValidator() = default;
		virtual ~SerializerValidator() = default;

		void onError(SerializerErrorData data);
		void validateOrThrow();
		void validateOrAbort() noexcept;

		static void DisableAbort();

	private:
		static bool ShouldAbort;

		template <class Function>
		bool validate(Function&& callback) {
			auto errors = m_errors;
			m_errors = {};
			if (!errors.empty()) {
				std::forward<Function>(callback)(std::move(errors));				
				return false;
			}
			return true;
		}

		std::vector<SerializerErrorData> m_errors;
	};
}