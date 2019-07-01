#include <cassert>
#include "Script/__internalConfig/LoggerConfig.h"
#include "CommandLogical.h"
#include "Core/Data/MemoryScript.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../ScriptUtils.h"

ska::CommandLogical::CommandLogical(EntityManager& entityManager) :
	AbstractFunctionCommand(entityManager) {
	//TODO gestion des parenth�ses
	static const constexpr char* Operators[] = { "&&", "||", "!=", "==", "<=", "<", ">", ">=" };
	
	/*
	m_operators.emplace(Operators[0], LogicalOperator { [](auto op1, auto op2) {
		return op1 && op2;
	}, 100 });

	m_operators.emplace(Operators[1], LogicalOperator { [](auto op1, auto op2) {
		return op1 || op2;
	}, 50 });

	m_operators.emplace(Operators[2], LogicalOperator { [](auto op1, auto op2) {
		return op1 != op2;
	}, 0 });

	m_operators.emplace(Operators[3], LogicalOperator { [](auto op1, auto op2) {
		return op1 == op2;
	}, 0 });

	m_operators.emplace(Operators[4], LogicalOperator { [](auto op1, auto op2) {
		return op1 <= op2;
	}, 1000 });

	m_operators.emplace(Operators[5], LogicalOperator{ [](auto op1, auto op2) {
		return op1 < op2;
	}, 1000 });

	m_operators.emplace(Operators[6], LogicalOperator{ [](auto op1, auto op2) {
		return op1 > op2;
	}, 1000 });

	m_operators.emplace(Operators[7], LogicalOperator{ [](auto op1, auto op2) {
		return op1 >= op2;
	}, 1000 });

	*/
}

int ska::CommandLogical::argumentsNumber() {
	return -1;
}
/*
namespace ska {
	void BuildAST(ASTNode& node, const std::vector<std::string>& expressionParts, const std::unordered_map<std::string, LogicalOperator>& operators) {
		auto maxPriorityLogicalOperator = std::string{};
		auto maxPriorityLogicalOperatorValue = -1;
		auto maxPriorityLogicalOperatorIndex = -1;

		const auto& maxRange = node.rangeMax == -1 ? expressionParts.size() - 1 : node.rangeMax;
		for (auto index = static_cast<std::size_t>(node.rangeMin); index <= maxRange; index++) {
			const auto& part = expressionParts[index];
			if (operators.find(part) != operators.end()) {
				if (operators.at(part).priority > maxPriorityLogicalOperatorValue) {
					maxPriorityLogicalOperator = part;
					maxPriorityLogicalOperatorValue = operators.at(part).priority;
					maxPriorityLogicalOperatorIndex = index;
				}
			}
		}
		
		if (maxPriorityLogicalOperatorIndex >= 0 && static_cast<std::size_t>(maxPriorityLogicalOperatorIndex) < expressionParts.size()) {
			node.type = OperatorType::Logical;
			node.operator_ = maxPriorityLogicalOperator;
			node.childLeft = std::make_unique<ASTNode>();
			node.childLeft->rangeMin = node.rangeMin;
			node.childLeft->rangeMax = maxPriorityLogicalOperatorIndex - 1;

			node.childRight = std::make_unique<ASTNode>();
			node.childRight->rangeMin = maxPriorityLogicalOperatorIndex + 1;
			node.childRight->rangeMax = node.rangeMax;
			BuildAST(*node.childLeft, expressionParts, operators);
			BuildAST(*node.childRight, expressionParts, operators);
		} else {			
			node.type = OperatorType::Numeric;
			node.value = StringUtils::fromString<int>(expressionParts[node.rangeMin]);
		}
	}

	ASTNode CreateAST(const std::vector<std::string>& expressionParts, const std::unordered_map<std::string, LogicalOperator>& operators) {
		auto root = ASTNode{};
		BuildAST(root, expressionParts, operators);
		return root;
	}

	int InterpretAST(const ASTNode& ast, const ScriptComponent& script, const MemoryScript& memory, const std::unordered_map<std::string, LogicalOperator>& operators) {
		if (ast.type == OperatorType::Logical) {
			assert(ast.childLeft != nullptr);
			auto left = InterpretAST(*ast.childLeft, script, memory, operators);

			assert(ast.childRight != nullptr);
			auto right = InterpretAST(*ast.childRight, script, memory, operators);
			
			return operators.at(ast.operator_).check(left, right);
		}
		return StringUtils::fromString<int>(ScriptUtils::replaceVariablesByNumerics(memory, script, ast.value));
	}
}
*/
std::string ska::CommandLogical::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	/*
	const auto& expression = args[0];
	const auto expressionParts = StringUtils::split(expression, ' ');
	auto ast = CreateAST(expressionParts, m_operators);
	auto result = InterpretAST(ast, script, memory, m_operators);

	return ska::StringUtils::intToStr(result);
	*/
	return "";
}
