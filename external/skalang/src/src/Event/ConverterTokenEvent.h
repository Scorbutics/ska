#pragma once
#include <cassert>

namespace ska {
	class ASTNode;
	class ScriptAST;

	class ConverterTokenEvent {
	public:
		ConverterTokenEvent(ASTNode& node, ScriptAST& script) : node(node), m_script(script) { assert(node.size() == 2 && node.op() == Operator::CONVERTER_CALL); }

		const auto& object() const { return node[0]; }
		const auto& type() const { return node[1]; }

		auto& script() { return m_script; }

		auto& rootNode() { return node; }

	private:
    	ASTNode& node;
		ScriptAST& m_script;
	};
}
