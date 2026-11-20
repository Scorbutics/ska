#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFieldAccess.h"
#include "NodeValue/ScriptAST.h"
#include "Service/ScopedSymbolTable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>::generate(OperateOn node, GenerationContext& context) {
	const auto& fieldName = node.GetFieldNameNode().name();

	const auto* objectTypeSymbolTable = node.GetObjectNameNode().symbolTable();
	if (objectTypeSymbolTable == nullptr) {
		throw std::runtime_error("unable to retrieve object type");
	}

	// Handling built-in direct fields
	const auto typeObject = node.GetObjectType();
	if (typeObject == ExpressionType::ARRAY) {
		if (fieldName == "size") {
			auto objectValue = generateNext(context.next(node.GetObjectNameNode()));
			objectValue.push({ Instruction{ Command::ARR_LENGTH, context.queryNextRegister(), objectValue.operand() } });
			return objectValue;
		}
		auto ss = std::stringstream{};
		ss << "trying to access an undeclared built-in field \"" << fieldName << "\" of the type \"" << typeObject << "\"";
		throw std::runtime_error(ss.str());
	}

	const auto index = FieldAccessIndex(node, context);
	auto objectValue = generateNext(context.next(node.GetObjectNameNode()));

	LOG_DEBUG << "This field has index " << index << " in the object";

	objectValue.push({ Instruction { Command::ARR_ACCESS, context.queryNextRegister(), objectValue.operand(), index }});
	return objectValue;
}

ska::bytecode::Operand ska::bytecode::FieldAccessIndex(OperationType<Operator::FIELD_ACCESS> node, const GenerationContext& context) {
	const auto& fieldName = node.GetFieldNameNode().name();
	const auto* objectTypeSymbolTable = node.GetObjectNameNode().symbolTable();
	if (objectTypeSymbolTable == nullptr) {
		throw std::runtime_error("unable to retrieve object type");
	}

	// Handling custom types
	const auto* symbolField = (*objectTypeSymbolTable)(fieldName);
	if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : \"" << fieldName << "\" of \"" << node.GetObjectNameNode().name() << "\"";
		throw std::runtime_error(ss.str());
	}

	const auto* objectSymbolInfo = context.getSymbolInfo(*symbolField->symbol());
	if (objectSymbolInfo == nullptr) {
		auto ss = std::stringstream { };
		ss << "invalid bytecode : the dereferenced object \"" << node.GetObjectNameNode() << "\" is not registered in script ";
		ss << context.scriptName();
		throw std::runtime_error(ss.str());
	}

	const auto index = objectTypeSymbolTable->id(*symbolField->symbol()).value();
	return Operand { static_cast<long>(index), OperandType::PURE };
}
