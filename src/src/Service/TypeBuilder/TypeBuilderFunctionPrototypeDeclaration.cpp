#include "TypeBuilderFunctionPrototypeDeclaration.h"
#include "Config/LoggerConfigLang.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

#include "Operation/OperationTypeType.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)
#define LOG_WARN SLOG_STATIC(ska::LogLevel::Warn, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)

namespace ska {
	static TypeHierarchy BuildObjectTypeHierarchy(const ASTNode& node, const ScopedSymbolTable& symbolTableFunction) {
		if (node.symbol() == nullptr) {
			return { Type::MakeCustom<ExpressionType::OBJECT>(&symbolTableFunction), symbolTableFunction };
		}
		return { Type::MakeCustom<ExpressionType::OBJECT>(&symbolTableFunction), &node };
	}

	TypeHierarchy TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(const ASTNode& node, const SymbolTable& symbolTable, const std::string functionName) {
		const auto type = node.type().value();
		const auto* symbolTableFunction = symbolTable[functionName];
		const auto objectIsVar = type == ExpressionType::OBJECT;
		if (objectIsVar) {
			LOG_INFO << "function prototype declaration has an OBJECT return type \""<< (symbolTableFunction != nullptr ? symbolTableFunction->name() : "") << "\"";
			const auto objectDoesntRefersToItself = node.symbol() != nullptr;
			if (!objectDoesntRefersToItself) {
				LOG_WARN << "Linking current function return type to master symbol \"" << (symbolTableFunction != nullptr ? symbolTableFunction->name() : "") << "\"";
				return BuildObjectTypeHierarchy(node, *symbolTableFunction);
			}

			const auto symbolType = node.symbol() == nullptr ? std::optional<Type>{} : node.symbol()->type();
			if (!symbolType.has_value()) {
				throw std::runtime_error("unable to find type symbol type of function");
			}

			LOG_INFO << "function symbol type deduced from type \""<<  symbolType.value() << "\"";
			if (symbolType.value() == ExpressionType::VOID) {
				assert(node.symbol() != nullptr);
				LOG_WARN << "Linking current function return type to master symbol \"" << (symbolTableFunction != nullptr ? symbolTableFunction->name() : "") << "\"";
				return BuildObjectTypeHierarchy(node, *symbolTableFunction);
			}

			if (symbolType.value().empty()) {
				auto ss = std::stringstream{};
				ss << "bad function type encountered \"" << symbolType.value() << "\": could not deduce its return value because it's empty";
				throw std::runtime_error(ss.str());
			}
			// Link to the returned object type's own (class) symbol table, NOT to the symbol table of the node (which is
			// the factory *function* table) : otherwise the current function would implement the factory function table,
			// and inside its body, looking up the factory name would resolve to the factory object child table ("var X").
			const auto& finalType = symbolType.value().back();
			return TypeHierarchy{ finalType };
		}

		if (type == ExpressionType::FUNCTION) {
			LOG_INFO << "function prototype declaration has a FUNCTION return type \""<< (symbolTableFunction != nullptr ? symbolTableFunction->name() : "") << "\"";
			assert(type.size() != 0);
			return type.back();
		}

		LOG_INFO << "function prototype declaration has a return type \"" << type << "\" for function \"" << (symbolTableFunction != nullptr ? symbolTableFunction->name() : "") << "\"";
		return type;
	}
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	auto functionName = node.GetFunctionName();
    auto& symbols = script.symbols();
	const ScopedSymbolTable* returnSymbolTable = nullptr;
	auto functionType = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
	std::size_t index = 0;
    for (auto& paramNode : node) {
			LOG_INFO << paramNode->name();
		if (index == node.GetParameterSize()) {
			auto hierarchy = TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(*paramNode, symbols, functionName);
			returnSymbolTable = hierarchy.link();
			functionType.add(std::move(hierarchy.type));
		} else {
			auto type = paramNode->type().value();
			functionType.add(std::move(type));
		}
		index++;
    }

		LOG_DEBUG << "function prototype declaration \""<< node.GetFunctionName() <<"\" with type "<< functionType;
	return returnSymbolTable != nullptr ? TypeHierarchy{ functionType, *returnSymbolTable } : TypeHierarchy{ functionType };
}
