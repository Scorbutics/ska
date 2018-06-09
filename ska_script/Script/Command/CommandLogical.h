#pragma once
#include <unordered_set>
#include <functional>
#include "AbstractFunctionCommand.h"

namespace ska {
	enum class OperatorType {
		Numeric,
		Logical,
		Root
	};
	
	struct ASTNode {
		OperatorType type = OperatorType::Root;
		std::string value;
		std::string operator_;
		std::unique_ptr<ASTNode> childLeft;
		std::unique_ptr<ASTNode> childRight;
		int rangeMin = 0;
		int rangeMax = -1;
	};

	struct LogicalOperator {
		using ComparisonPredicate = std::function<bool(int, int)>;

		ComparisonPredicate check;
		int priority;
	};

	class CommandLogical :
		public AbstractFunctionCommand {
		
	public:
		explicit CommandLogical(EntityManager& e);
		void operator=(const CommandLogical&) = delete;
		virtual ~CommandLogical() = default;

		virtual std::string execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	private:
		std::unordered_map<std::string, LogicalOperator> m_operators;
	};
}