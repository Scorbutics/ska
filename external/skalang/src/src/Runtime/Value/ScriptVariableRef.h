#pragma once
#include <ostream>
#include <memory>
namespace ska {
	class PlainMemoryTable;

	struct VariableRef {
		std::size_t variable = 0;
	};

	struct ScriptVariableRef {
		ScriptVariableRef();
		ScriptVariableRef(std::size_t variable, std::size_t script);
		virtual ~ScriptVariableRef();
		std::size_t variable = 0;
		std::size_t script = 0;
	};

	bool operator==(const VariableRef& lhs, const VariableRef& rhs);
	bool operator==(const ScriptVariableRef& lhs, const ScriptVariableRef& rhs);

	std::ostream& operator<<(std::ostream& stream, const ScriptVariableRef& var);
}

namespace std {
	template<>
	struct hash<ska::ScriptVariableRef> {
		size_t operator()(const ska::ScriptVariableRef & x) const {
			const size_t h1 = hash<std::size_t>()(x.script);
			const size_t h2 = hash<std::size_t>()(x.variable);
			return h1 ^ (h2 << 1);
		}
	};

	template<>
	struct hash<ska::VariableRef> {
		size_t operator()(const ska::VariableRef& x) const {
			return hash<std::size_t>()(x.variable);
		}
	};
}
