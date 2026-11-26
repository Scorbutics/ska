#include "PlainMemoryTable.h"

namespace ska {
	std::ostream& operator<<(std::ostream& stream, const PlainMemoryTable& table) {
		stream << "[";
		for (const auto& value: table.m_data) {
			stream << value;
			stream << ", ";
		}
		stream << "]";
		return stream;
	}
}
