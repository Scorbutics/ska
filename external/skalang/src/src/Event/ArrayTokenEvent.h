#pragma once

namespace ska {
	class ASTNode;
	class ScriptAST;

	enum class ArrayTokenEventType {
    	DECLARATION,
		EXPRESSION,
    	USE
	};

	class ArrayTokenEvent {
	public:
		ArrayTokenEvent(ASTNode& node, ScriptAST& s, ArrayTokenEventType type) : m_node(node), m_type(type), m_script(s) {}

		auto& rootNode() {
			return m_node;
		}

		const auto& rootNode() const {
			return m_node;
		}

		const auto& type() const {
			return m_type;
		}
		
		ScriptAST& script() {
			return m_script;
		}

	private:
		ArrayTokenEventType m_type;
		ASTNode& m_node;
		ScriptAST& m_script;
	};
}
