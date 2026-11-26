#pragma once
#include <cassert>
#include "NodeValue/Operator.h"
#include "NodeValue/ScriptASTPtr.h"

namespace ska {
	class ScriptAST;
	class ASTNode;

	class ScriptLinkTokenEvent {
	public:
		ScriptLinkTokenEvent(ASTNode& node, std::string scriptName, ScriptAST& scriptCaller);

		ScriptAST& script();
		const ScriptAST& script() const;

		ScriptAST* bound() { return m_script.get(); }
		const ScriptAST* bound() const { return m_script.get(); }

		ASTNode& rootNode();
		const ASTNode& rootNode() const;

		const std::string& name() const { return m_name; }

	private:
		ASTNode& m_node;
		std::string m_name;
		ScriptAST& m_scriptCaller;
		ScriptASTPtr m_script;		
	};
}
