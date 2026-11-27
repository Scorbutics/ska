#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "Service/ScopedSymbolTable.h"
#include "GeneratorOperatorLanguageValues.h"
#include "ComputingOperations/BytecodeTypeConversion.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

std::vector<std::string> ska::bytecode::EnumValueNames(const ScopedSymbolTable& enumTable) {
	auto names = std::vector<std::string>{};
	for (const auto& value : enumTable) {
		if (value != nullptr && value->symbol() != nullptr) {
			names.push_back(value->name());
		}
	}
	return names;
}

ska::bytecode::Operand ska::bytecode::StringConstant(const std::string& text) {
	return Operand{ std::make_shared<std::string>(text), OperandType::PURE };
}

ska::bytecode::InstructionOutput ska::bytecode::GenerateStringArray(const std::vector<std::string>& names, GenerationContext& context) {
	auto result = InstructionOutput{};
	auto pushed = std::vector<OperandUse>{};
	for (const auto& name : names) {
		pushed.push_back(StringConstant(name));
		if (pushed.size() == 3) {
			result.push(Instruction{ Command::PUSH, std::move(pushed) });
			pushed = {};
		}
	}
	if (!pushed.empty()) {
		result.push(Instruction{ Command::PUSH, std::move(pushed) });
	}
	result.push(Instruction{ Command::POP_IN_ARR, context.queryNextRegister(), Operand{ static_cast<long>(names.size()), OperandType::PURE } });
	return result;
}

// MAP_NEW, then MAP_SET of each entry
ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::MAP_LITERAL>::generate(OperateOn node, GenerationContext& context) {
	const auto map = context.queryNextRegister();
	auto result = InstructionOutput{ Instruction{ Command::MAP_NEW, map } };
	for (std::size_t index = 0; index < node.size(); index += 2) {
		auto key = generateNext(context.next(node[index]));
		const auto keyOperand = key.operand();
		result.push(std::move(key));
		auto value = generateNext(context.next(node[index + 1]));
		const auto valueOperand = value.operand();
		result.push(std::move(value));
		result.push(Instruction{ Command::MAP_SET, map, keyOperand, valueOperand });
	}
	result.push(map);
	return result;
}

// The value of the enum variable : the names of its values (a string array). A value is its name ("Status.poison" is
// the constant "poison").
ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::ENUM_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	auto names = std::vector<std::string>{};
	for (std::size_t index = 0; index < node.size(); index++) {
		names.push_back(node[index].name());
	}
	return GenerateStringArray(names, context);
}

// The subject in a register, then each branch : its tests (a jump to its result when a value is equal to the subject),
// a jump over its result when none is, its result (moved into the result register) and a jump to the end.
ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::MATCH>::generate(OperateOn node, GenerationContext& context) {
	const auto& subjectNode = node[0];
	const auto& subjectType = subjectNode.type().value();
	auto result = generateNext(context.next(subjectNode));
	const auto subject = context.queryNextRegister();
	result.push(Instruction{ Command::MOV, subject, result.operand() });
	const auto resultRegister = context.queryNextRegister();

	struct Branch {
		std::vector<InstructionOutput> tests;
		InstructionOutput value;
	};
	auto branches = std::vector<Branch>{};
	for (std::size_t index = 1; index < node.size(); index++) {
		const auto& branchNode = node[index];
		auto branch = Branch{};
		for (std::size_t valueIndex = 1; valueIndex < branchNode.size(); valueIndex++) {
			const auto& valueNode = branchNode[valueIndex];
			auto test = generateNext(context.next(valueNode));
			const auto valueOperand = test.operand();
			const auto equal = context.queryNextRegister();
			test.push(TypeConversionBinary(LogicalOperator::EQUALITY,
				{ subjectType, subject }, { valueNode.type().value(), valueOperand }, { Type::MakeBuiltIn<ExpressionType::BOOLEAN>(), equal }));
			test.push(Instruction{ Command::JUMP_NIF, Operand{ 1L, OperandType::PURE }, equal });
			branch.tests.push_back(std::move(test));
		}
		auto value = generateNext(context.next(branchNode[0]));
		value.push(Instruction{ Command::MOV, resultRegister, value.operand() });
		branch.value = std::move(value);
		branches.push_back(std::move(branch));
	}

	// Assembled from the last branch : the jumps need the sizes of what follows them
	auto tail = InstructionOutput{};
	for (auto branchIt = branches.rbegin(); branchIt != branches.rend(); ++branchIt) {
		auto& branch = *branchIt;
		auto assembled = InstructionOutput{};
		if (branch.tests.empty()) {
			// else
			assembled.push(std::move(branch.value));
		} else {
			branch.value.push(Instruction{ Command::JUMP_REL, Operand{ static_cast<long>(tail.size()), OperandType::PURE } });
			// After a test : the remaining tests and the jump over the result
			auto remaining = std::vector<std::size_t>(branch.tests.size(), 1u);
			for (auto testIndex = branch.tests.size(); testIndex-- > 1;) {
				remaining[testIndex - 1] = remaining[testIndex] + branch.tests[testIndex].size() + 1;
			}
			for (std::size_t testIndex = 0; testIndex < branch.tests.size(); testIndex++) {
				assembled.push(std::move(branch.tests[testIndex]));
				// Equal : to the result
				assembled.push(Instruction{ Command::JUMP_REL, Operand{ static_cast<long>(remaining[testIndex]), OperandType::PURE } });
			}
			assembled.push(Instruction{ Command::JUMP_REL, Operand{ static_cast<long>(branch.value.size()), OperandType::PURE } });
			assembled.push(std::move(branch.value));
		}
		assembled.push(std::move(tail));
		tail = std::move(assembled);
	}
	result.push(std::move(tail));
	result.push(resultRegister);
	return result;
}
