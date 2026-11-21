#include <iostream>
#include "Config/LoggerConfigLang.h"
#include "Service/StatementParser.h"
#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/OperatorTraits.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/Type.h"

#include "Operation/OperationTypeFunctionDeclaration.h"
#include "Operation/OperationTypeFunctionCall.h"
#include "Operation/OperationTypeIfElse.h"
#include "Operation/OperationTypeArrayDeclaration.h"
#include "Operation/OperationTypeReturn.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SemanticTypeChecker)

ska::SemanticTypeChecker::SemanticTypeChecker(StatementParser& parser, const TypeCrosser& typeCrosser) :
	subobserver_priority_queue<VarTokenEvent>(std::bind(&SemanticTypeChecker::matchVariable, this, std::placeholders::_1), parser, 9),
	subobserver_priority_queue<FunctionTokenEvent>(std::bind(&SemanticTypeChecker::matchFunction, this, std::placeholders::_1), parser, 9),
	subobserver_priority_queue<ArrayTokenEvent>(std::bind(&SemanticTypeChecker::matchArray, this, std::placeholders::_1), parser, 9),
	subobserver_priority_queue<ReturnTokenEvent>(std::bind(&SemanticTypeChecker::matchReturn, this, std::placeholders::_1), parser, 9),
	subobserver_priority_queue<IfElseTokenEvent>(std::bind(&SemanticTypeChecker::matchIfElse, this, std::placeholders::_1), parser, 9),
	subobserver_priority_queue<ConverterTokenEvent>(std::bind(&SemanticTypeChecker::matchConverter, this, std::placeholders::_1), parser, 9),
	m_typeCrosser(typeCrosser) {
}

bool ska::SemanticTypeChecker::statementHasReturnOnAllControlPath(const ASTNode& node) {
	switch (node.op()) {
	case Operator::FUNCTION_DECLARATION:
	case Operator::FUNCTION_PROTOTYPE_DECLARATION:
	case Operator::IF:
		//false, cause even if there was a return inside children nodes, in this case it would not be enough to guaranty the systematic return
		return false;
	case Operator::IF_ELSE: {
		auto ifElseOperation = OperationType<Operator::IF_ELSE>{ node };
		return statementHasReturnOnAllControlPath(ifElseOperation.GetIfStatement()) && statementHasReturnOnAllControlPath(ifElseOperation.GetElseStatement());
	}
	case Operator::RETURN:
		return true;
	default:
		return childrenHasReturnOnAllControlPath(node);
	}
}

bool ska::SemanticTypeChecker::childrenHasReturnOnAllControlPath(const ASTNode& node) {
	auto result = false;
	for (const auto& statement : node) {
		result |= statementHasReturnOnAllControlPath(*statement);
		if (result) {
			break;
		}
	}

	return result;
}

bool ska::SemanticTypeChecker::matchReturn(const ReturnTokenEvent& token) {
	switch(token.type()) {
	case ReturnTokenEventType::START:
		break;

	case ReturnTokenEventType::BUILTIN:
	case ReturnTokenEventType::OBJECT: {
		auto operationReturn = OperationType<Operator::RETURN>{token.rootNode()};
		const auto& returnedValue = operationReturn.GetValue();

		// Latest declared children symbol table correspond to either function scope or inner return scope
		const auto* lastScopedSymbolTable = returnedValue.op() == Operator::SCRIPT_OBJECT ? &token.script().symbols().current() :
			token.script().symbols().current().back();
		// Get the return function owner (original function symbol)
		const auto* owner = lastScopedSymbolTable == nullptr ? nullptr : lastScopedSymbolTable->owner();
		if (owner == nullptr || owner->name().empty()) {
			throw std::runtime_error("return must be place in a function block or a nested one");
		}
		if (!returnedValue.type().has_value()) {
			throw std::runtime_error("\"" + owner->name() + "\" is not a function");
		}

		if (returnedValue.type().value() == ExpressionType::VOID) {
			throw std::runtime_error("return cannot be used for the void type");
		}

		if (owner->symbol()->type().empty()) {
			break;
		}

		// Script objects are directly containing all fields
		// Classic returned objects expect the last compound type as return type
		const auto expectedReturnType = returnedValue.op() == Operator::SCRIPT_OBJECT ? owner->symbol()->type() : owner->symbol()->type().back();

		if (((returnedValue.op() == Operator::USER_DEFINED_OBJECT) && (expectedReturnType != ExpressionType::OBJECT)) ||
			(returnedValue.op() != Operator::USER_DEFINED_OBJECT && expectedReturnType != returnedValue.type())) {
			auto ss = std::stringstream{};
			ss << "bad return type : expected \"" << expectedReturnType << "\" on function declaration but got \"" << returnedValue.type().value() << "\" on return";
			throw std::runtime_error(ss.str());
		}
	} break;

	default:
		break;
	}
	return true;
}

bool ska::SemanticTypeChecker::matchIfElse(const IfElseTokenEvent& token) {
	const auto& ifElseOperation = OperationType<Operator::IF_ELSE>{token.rootNode()};
	const auto& conditionType = ifElseOperation.GetCondition().type().value_or(Type{});
	if (conditionType != ExpressionType::BOOLEAN) {
		auto ss = std::stringstream{};
		ss << "expression condition is not a boolean (it's a \"" << conditionType << "\")";
		throw std::runtime_error(ss.str());
	}
	return true;
}

bool ska::SemanticTypeChecker::matchArray(const ArrayTokenEvent& token) {
	switch (token.type()) {
	case ArrayTokenEventType::DECLARATION: {
		// If and only if an explicit array type does not exists, we have array content. So semantic checks are required.
		if (token.rootNode()[1].logicalEmpty()) {
			auto arrayDeclarationOperation = OperationType<Operator::ARRAY_DECLARATION>{token.rootNode()[0]};
			const auto arraySubType = arrayDeclarationOperation.GetArraySubType();
			for (const auto& arrayElementNode : arrayDeclarationOperation) {
				if (arraySubType != arrayElementNode->type()) {
					auto ss = std::stringstream{};
					ss << "array has not uniform types in it : \"" << arraySubType << "\" and \"" << arrayElementNode->type().value() << "\"";
					throw std::runtime_error(ss.str());
				}
			}
		}
	} break;

	case ArrayTokenEventType::EXPRESSION: {
		if(token.rootNode().type() != ExpressionType::ARRAY) {
			auto ss = std::stringstream {};
			ss << "expression is not an array (it's a \"" << token.rootNode().type().value_or(Type {}) << "\")";
			throw std::runtime_error(ss.str());
		}
	} break;

	case ArrayTokenEventType::USE:
		break;

	default:
		assert(!"Unhandled event type");
		break;
	}

	return true;
}

template <class Operation>
static void CommonMatchFunctionCall(const ska::TypeCrosser& typeCrosser, Operation functionCallOperation, unsigned int parametersSize) {
	const auto functionFullRequiredType = functionCallOperation.GetFunctionType();

	if (functionFullRequiredType != ska::ExpressionType::FUNCTION) {
		auto ss = std::stringstream{};
		ss << "function \"" << functionFullRequiredType << "\" is called before being declared (or has a bad declaration)";
		throw std::runtime_error(ss.str());
	}

	const auto functionRequiredTypeParameterSize = functionFullRequiredType.size() - parametersSize;
	const auto callNodeParameterSize = functionCallOperation.GetFunctionParameterSize();
	if (functionRequiredTypeParameterSize != callNodeParameterSize) {
		auto ss = std::stringstream{};
		ss << "bad function call : the function \"" << functionFullRequiredType << "\" needs "
			<< functionRequiredTypeParameterSize << " parameters but is being called with " << callNodeParameterSize
			<< " parameters";
		throw std::runtime_error(ss.str());
	}

	SLOG_STATIC(ska::LogLevel::Debug, ska::SemanticTypeChecker) << functionFullRequiredType << " function has the following arguments types during its call : ";
	auto index = static_cast<std::size_t>(parametersSize - 1);
	for (auto& arg : functionCallOperation) {
		const auto calculatedArgumentType = arg->type().value();
		const auto requiredParameterType = functionFullRequiredType[index];

		if (requiredParameterType.crossTypes(typeCrosser, "=", calculatedArgumentType) == ska::ExpressionType::VOID) {
			auto ss = std::stringstream{};
			ss << "Type \"" << calculatedArgumentType << "\" is encountered while a type convertible to \""
				<< requiredParameterType << "\" is required";
			throw std::runtime_error(ss.str());
		}
		index++;
	}
}

void ska::SemanticTypeChecker::matchFunctionCall(const FunctionTokenEvent& token) {
	auto functionCallOperation = OperationType<Operator::FUNCTION_CALL>{ token.rootNode() };
	CommonMatchFunctionCall(m_typeCrosser, functionCallOperation, 1);
}

bool ska::SemanticTypeChecker::matchFunction(const FunctionTokenEvent& token) {
	switch(token.type()) {

	case FunctionTokenEventType::CALL: {
		matchFunctionCall(token);
	} break;


	case FunctionTokenEventType::DECLARATION_STATEMENT: {
		auto operation = OperationType<Operator::FUNCTION_DECLARATION>{ token.rootNode() };
		auto functionReturnType = operation.GetFunctionPrototype().type().value().back();
		if (functionReturnType != ExpressionType::VOID &&
			!childrenHasReturnOnAllControlPath(operation.GetFunctionBody())) {
			throw std::runtime_error("function lacks of return in one of its code path");
		}
	} break;

	default:
		break;
	}

	return true;
}

bool ska::SemanticTypeChecker::matchVariable(const VarTokenEvent& variable) {
	assert(variable.varType().has_value() && "The current variable has no type. Maybe you forgot to add a type builder ?");
	const auto value = variable.value();
	const auto name = variable.name();
	const auto type = variable.varType().value();

	SLOG(ska::LogLevel::Debug) << name << " = " << value << ";\tsymbol = " << type;

	if(type == ExpressionType::VOID) {
		throw std::runtime_error("The symbol \"" + name + "\" cannot be declared as a void type");
	}

	if (variable.type() == VarTokenEventType::DECLARATION &&
		OperatorTraits::isNamed(variable.rootNode().op()) && name.empty()) {
		throw std::runtime_error("invalid symbol declaration");
	}

	if(variable.type() == VarTokenEventType::AFFECTATION || variable.type() == VarTokenEventType::DECLARATION) {
		if (!OperatorTraits::isLvalueCompatible(variable.var().op())) {
			auto ss = std::stringstream{};
			ss << "The symbol \"" << name << "\" is not an lvalue, therefore cannot be assigned";
			throw std::runtime_error(ss.str());
		}

		if (variable.var().symbol() == nullptr) {
			auto ss = std::stringstream{};
			ss << "The left part of assignation is a read-only value, therefore cannot be assigned";
			throw std::runtime_error(ss.str());
		}

		if (variable.valType().has_value()) {
			const auto tokenNodeExpressionType = variable.valType().value();
			const auto newTokenType = type.crossTypes(m_typeCrosser, "=", tokenNodeExpressionType);
			if (newTokenType == ExpressionType::VOID) {
				const auto expressionTypeIndex = tokenNodeExpressionType;
				auto ss = std::stringstream{};
				ss << "The symbol \"" << name << "\" has already been declared as \"" <<
					type << "\" but is now wanted to be \"" <<
					expressionTypeIndex << "\"";
				throw std::runtime_error(ss.str());
			}
		}

		if (OperatorTraits::isNamed(variable.var().op()) && name.empty()) {
			throw std::runtime_error("invalid symbol affectation");
		}
	}

	return true;
}

bool ska::SemanticTypeChecker::matchConverter(const ConverterTokenEvent& token) {
	const auto& sourceType = token.object().type().value();
	const auto& destinationType = token.type().type().value();

	if (ExpressionTypeIsBuiltIn(sourceType.type()) && !ExpressionTypeIsBuiltIn(destinationType.type())) {
		auto ss = std::stringstream{};
		ss << "invalid conversion from a built-in type \"" << sourceType << "\" to type \"" << destinationType << "\"";
		throw std::runtime_error(ss.str());
	}

	return true;
}