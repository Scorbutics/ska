#include <cassert>
#include "Config/LoggerConfigLang.h"
#include "ImportTokenEvent.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/AST.h"
#include "Service/ScriptNameBuilder.h"

ska::ImportTokenEvent::ImportTokenEvent(ASTNode& node, ScriptAST& s) :
	m_node(node),
	m_script(s),
	m_boundScript(m_script.useImport(ScriptNameDeduce(s.name(), node[0].name()))) {
	assert(m_boundScript != nullptr && "bad parameter : null bound script to an import event");
}
