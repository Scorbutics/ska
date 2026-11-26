#include "NodeValue/AST.h"
#include "VarTokenEvent.h"

std::string ska::VarTokenEvent::name() const
{
		switch (m_type) {
		case VarTokenEventType::AFFECTATION:
			return m_node[0].name();
		default:
			return m_node.name();
		}
	
}

const std::optional<ska::Type>& ska::VarTokenEvent::varType() const {
	return m_node.type();
}

std::optional<ska::Type> ska::VarTokenEvent::valType() const {
	if (m_node.size() >= 2) {
		return m_node[1].type();
	}
	return m_node.size() > 0 ? std::optional<ska::Type>{m_node[0].type()} : std::optional<ska::Type>{};
}

std::string ska::VarTokenEvent::value() const {
	if (m_node.size() > 0) {
		return m_node[0].name();
	}
	return "";
}

const ska::ASTNode& ska::VarTokenEvent::var() const {
	switch (m_type) {
		case VarTokenEventType::AFFECTATION:
			return m_node[0];
		default:
			return m_node;
	}
}

const ska::ASTNode* ska::VarTokenEvent::val() const {
	if (m_node.size() >= 2) {
		return &m_node[1];
	}
	return m_node.size() > 0 ? &m_node[0] : nullptr;
}
