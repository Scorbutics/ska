#include "BytecodeInstruction.h"

#define LOG_CMD
#define LOG_VALUE

std::ostream& ska::bytecode::operator<<(std::ostream& stream, const Instruction& cell) {
	stream << "["
	#ifdef LOG_CMD
	<< cell.m_command << "|"
	#endif
	#ifdef LOG_VALUE
	<< cell.m_dest.toString() << "|"
	<< cell.m_left.toString() << "|"
	<< cell.m_right.toString()
	#endif
	<< "]";
	return stream;
}

bool ska::bytecode::operator==(const ska::bytecode::Instruction& left, const ska::bytecode::Instruction& right) {
	return left.m_command == right.m_command && left.m_left == right.m_left && left.m_right == right.m_right && left.m_dest == right.m_dest;
}

bool ska::bytecode::operator!=(const ska::bytecode::Instruction& left, const ska::bytecode::Instruction& right) {
	return !operator==(left, right);
}