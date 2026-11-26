#pragma once
#include <cassert>
#include <string>
#include "NodeValue/ScriptASTPtr.h"

namespace ska {
	class ASTNode;
	class ScriptAST;
	
	class ImportTokenEvent {
	public:
		ImportTokenEvent(ASTNode& node, ScriptAST& s);
		
    	auto& rootNode() {
        	return m_node;
		}

		const auto& rootNode() const {
        	return m_node;
		}

		const ScriptAST& script() const {
			return m_script;
		}

		const ScriptAST& bound() const {
			return *m_boundScript;
		}

		ScriptAST& bound() {
			return *m_boundScript;
		}

	private:
		ASTNode& m_node;
    	ScriptAST& m_script;
		ScriptASTPtr m_boundScript;
	};
}
