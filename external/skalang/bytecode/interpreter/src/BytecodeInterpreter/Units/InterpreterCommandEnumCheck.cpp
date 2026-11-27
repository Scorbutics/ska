#include "InterpreterCommandEnumCheck.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ENUM_CHECK)(ExecutionContext& context, const Operand& left, const Operand& right) {
	// ENUM_CHECK dest string names : a string converted to a value of an enum (no null value : a text that is not one of
	// its names is an error)
	const auto text = context.get<StringShared>(left);
	auto names = context.get<NodeValueArray>(right);
	for (const auto& name : *names) {
		if (*name.nodeval<StringShared>() == *text) {
			return text;
		}
	}
	auto list = std::string{};
	for (const auto& name : *names) {
		list += (list.empty() ? "" : ", ") + *name.nodeval<StringShared>();
	}
	throw std::runtime_error("\"" + *text + "\" is not a value of the enum (its values : " + list + ")");
}
