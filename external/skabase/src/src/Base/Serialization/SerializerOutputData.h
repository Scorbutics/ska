#pragma once

#include <sstream>
#include "SerializerNatives.h"

namespace ska {
	class SerializerOutputData {
	public:
		SerializerOutputData(std::stringstream& buffer, SerializerNativeContainer& natives) :
			m_buffer(&buffer),
			m_natives(&natives) {
		}
		
		std::stringstream& buffer() { return *m_buffer; }
		SerializerNativeContainer& natives() { return *m_natives; }
		const SerializerNativeContainer& natives() const { return *m_natives; }

	private:
		std::stringstream* m_buffer;
		SerializerNativeContainer* m_natives;
	};
}