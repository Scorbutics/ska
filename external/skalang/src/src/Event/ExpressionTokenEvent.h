#pragma once

namespace ska {
	class ASTNode;
	class ScriptAST;

	class ExpressionTokenEvent {
	public:
		ExpressionTokenEvent(ASTNode& node, ScriptAST& s) : node(node), m_script(s) {}

		auto& rootNode() {
			return node;
		}

		const auto& rootNode() const {
			return node;
		}

		ScriptAST& script() {
			return m_script;
		}

	private:
		ASTNode& node;
		ScriptAST& m_script;
	};
}
