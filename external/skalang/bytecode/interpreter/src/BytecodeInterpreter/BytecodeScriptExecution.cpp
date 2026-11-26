#include "Config/LoggerConfigLang.h"
#include "BytecodeScriptExecution.h"
#include "BytecodeInterpreter/Value/BytecodeExecutor.h"

ska::NodeValue ska::bytecode::ScriptExecution::getCell(const Operand& v) const {
	if (v.empty()) {
		return {};
	}

	auto* memory = selectMemory(v);
	if (memory == nullptr) {
		auto output = NodeValue{};
		std::visit([&](const auto& content) {
			using TypeT = std::decay_t<decltype(content)>;
			if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
				auto ss = std::stringstream{};
				ss << "invalid cell : memory type " << v.type();
				throw std::runtime_error(ss.str());
			} else {
				output = content;
			}
		}, v.content());
		return output;
	}
	return (*memory)[v.as<ScriptVariableRef>().variable];
}

void ska::bytecode::ScriptExecution::jumpAbsolute(std::size_t value) {
	executionPointer = value;
}

void ska::bytecode::ScriptExecution::jumpRelative(long value) {
	executionPointer += value;
}

std::size_t ska::bytecode::ScriptExecution::id() const {
	return scriptIndex;
}

const std::string& ska::bytecode::ScriptExecution::name() const {
	return instructions.at(scriptIndex).name();
}

ska::PlainMemoryTable* ska::bytecode::ScriptExecution::selectMemory(const Operand& dest) {
	return SelectMemoryHelper<decltype(*this), PlainMemoryTable*>(*this, dest);
}

const ska::PlainMemoryTable* ska::bytecode::ScriptExecution::selectMemory(const Operand& dest) const {
	return SelectMemoryHelper<decltype(*this), const PlainMemoryTable*>(*this, dest);
}

ska::NodeValue ska::bytecode::ScriptExecution::lastVariable() const {
	assert(!variables.empty());
	return variables.back().resolved();
}

ska::NodeValue ska::bytecode::ScriptExecution::shareVariable(const Operand& variable) {
	if (variable.type() != OperandType::VAR) {
		throw std::runtime_error("invalid export : only a script level variable can be exported");
	}
	const auto index = variable.as<ScriptVariableRef>().variable;
	const auto* value = variables.get_if(index);
	if (value == nullptr) {
		throw std::runtime_error("invalid export : the script variable " + std::to_string(index) + " does not exist");
	}
	if (!value->isCellReference()) {
		auto cell = std::make_shared<NodeValue>(*value);
		variables.replace(index, NodeValue::CellReference(std::move(cell)));
	}
	return variables.raw(index);
}
