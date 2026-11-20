#include "Config/LoggerConfigLang.h"
#include "BytecodeInstructionOutput.h"
#include "BytecodeScriptGeneration.h"

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const InstructionPack& group) {
	for(const auto& c : group) {
		stream << c << "\n";
	}
	return stream;
}

ska::bytecode::InstructionOutput::InstructionOutput(ScriptGeneration script) :
	InstructionOutput(std::move(script.m_generated)) {
}

void ska::bytecode::InstructionOutput::push(InstructionOutput operand) {
	const auto wasEmpty = operand.m_pack.empty();
	if(!wasEmpty) {
		m_pack.insert(m_pack.end(), std::make_move_iterator(operand.m_pack.begin()), std::make_move_iterator(operand.m_pack.end()));
		m_operand = {};
	} else {
		m_operand = operand.m_operand;
	}

}

void ska::bytecode::InstructionOutput::push(OperandUse operand) {
	push(InstructionOutput {std::move(operand) });
}

void ska::bytecode::InstructionOutput::push(Instruction operand) {
	push(InstructionOutput {std::move(operand) });
}

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const InstructionOutput& output) {
	if(output.m_pack.empty()) {
		if(!output.m_operand.empty()) {
			stream << output.m_operand.toString();
		}
	} else {
		stream << output.m_pack << " ";
	}
	return stream;
}

ska::bytecode::Operand ska::bytecode::InstructionOutput::packAsOperand() const {
	if(m_pack.empty()) {
		return {};
	}

	return m_pack.back().dest();
}

bool ska::bytecode::operator==(const InstructionOutput& lhs, const InstructionOutput& rhs) {
	return lhs.m_pack == rhs.m_pack;
}

bool ska::bytecode::operator!=(const InstructionOutput& lhs, const InstructionOutput& rhs) {
	return !operator==(lhs, rhs);
}
