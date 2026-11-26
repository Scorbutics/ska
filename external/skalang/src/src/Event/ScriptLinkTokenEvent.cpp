#include "Config/LoggerConfigLang.h"
#include "ScriptLinkTokenEvent.h"
#include "NodeValue/ScriptAST.h"
#include "NodeValue/AST.h"

ska::ScriptLinkTokenEvent::ScriptLinkTokenEvent(ASTNode& node, std::string scriptName, ScriptAST& scriptCaller) :
	m_node(node),
	m_name(std::move(scriptName)),
	m_scriptCaller(scriptCaller),
	m_script(scriptCaller.useImport(m_name)) {
	assert(m_node.op() == Operator::SCRIPT_LINK);
}

ska::ScriptAST& ska::ScriptLinkTokenEvent::script() {
	return m_scriptCaller;
}

const ska::ScriptAST& ska::ScriptLinkTokenEvent::script() const {
	return m_scriptCaller;
}

ska::ASTNode& ska::ScriptLinkTokenEvent::rootNode() {
	return m_node;
}

const ska::ASTNode& ska::ScriptLinkTokenEvent::rootNode() const {
	return m_node;
}
