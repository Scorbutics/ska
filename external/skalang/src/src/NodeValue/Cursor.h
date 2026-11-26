#pragma once
#include <cstddef>

namespace ska {
  	using LineIndex = unsigned int;
	using ColumnIndex = unsigned int;
	struct Cursor {
    	std::size_t rawIndex = 0;
    	ColumnIndex column = 1;
    	LineIndex line = 1;
	};

	static inline bool operator==(const Cursor& c1, const Cursor& c2) { return c1.rawIndex == c2.rawIndex; }
	static inline bool operator!=(const Cursor& c1, const Cursor& c2) { return !(c1 == c2); }
	static inline bool operator<(const Cursor& c1, const Cursor& c2) { return c1.rawIndex < c2.rawIndex;  }
	static inline bool operator<=(const Cursor& c1, const Cursor& c2) { return !(c2.rawIndex < c1.rawIndex); }
	static inline bool operator>(const Cursor& c1, const Cursor& c2) { return c2.rawIndex < c1.rawIndex; }
	static inline bool operator>=(const Cursor& c1, const Cursor& c2) { return !(c1.rawIndex < c2.rawIndex); }
}
