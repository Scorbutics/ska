#include "GeneratorOperatorUnit.h"
#include "BytecodeGenerator.h"

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperatorBase::generateNext(GenerationContext node) {
	return m_generator.generatePart(std::move(node));
}
