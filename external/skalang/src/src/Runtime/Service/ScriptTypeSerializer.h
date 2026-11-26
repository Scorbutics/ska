#pragma once
#include <unordered_map>
#include <sstream>

namespace ska {
	class ScopedSymbolTable;
	struct Type;
	class SerializerOutput;

	class ScriptTypeSerializer {
	public:
		ScriptTypeSerializer() = default;
		virtual ~ScriptTypeSerializer() = default;

		virtual void write(SerializerOutput& output, const ScopedSymbolTable* symbol, const Type& type) = 0;
	};
}