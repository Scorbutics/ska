#include "NodeValue.h"
#include "PlainMemoryTable.h"
#include "ScriptVariableRef.h"

std::ostream& ska::operator<<(std::ostream& stream, const ScriptVariableRef& var) {
	stream << var.variable << ":" << var.script;
	return stream;
}

bool ska::operator==(const ScriptVariableRef& lhs, const ScriptVariableRef& rhs) {
	return lhs.script == rhs.script && lhs.variable == rhs.variable;
}

bool ska::operator==(const VariableRef& lhs, const VariableRef& rhs) {
	return lhs.variable == rhs.variable;
}

ska::ScriptVariableRef::ScriptVariableRef(std::size_t variable, std::size_t script):
	variable(variable),
	script(script) {
}

ska::ScriptVariableRef::ScriptVariableRef() {
}

ska::ScriptVariableRef::~ScriptVariableRef() { }
