#include "Config/LoggerConfigLang.h"
#include "AST.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/TypeBuilder/TypeBuildersContainer.h"
#include "Service/ScopedSymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ASTNode)

ska::ASTNode::ASTNode() :
	m_type(Type::MakeBuiltIn(ExpressionType::VOID)) {}

ska::ASTNode::ASTNode(Token t, ASTNodePtr l, ASTNodePtr r) :
	m_op(l != nullptr && r != nullptr ? Operator::BINARY : Operator::UNARY),
	m_token(std::move(t)) {
	if (l != nullptr) {
		m_children.push_back(std::move(l));
	}

	if (r != nullptr) {
		m_children.push_back(std::move(r));
	}

	if(m_token.isLiteral()) {
		assert(m_op == Operator::UNARY);
		m_op = Operator::LITERAL;
	}
}

ska::ASTNode:: ASTNode(Operator o, Token identifierToken, std::vector<ASTNodePtr> children) :
	m_op(o),
	m_token(std::move(identifierToken)) {
	if(!children.empty()) {
		m_children.reserve(children.size());
		for(auto& child : children) {
			if(child != nullptr) {
				m_children.push_back(std::move(child));
			}
		}
	}
}

ska::ASTNode::ASTNode(Operator o, Token identifierToken, std::deque<ASTNodePtr> children) :
	m_op(o),
	m_token(std::move(identifierToken)) {
	if (!children.empty()) {
		m_children.reserve(children.size());
		for (auto& child : children) {
			if (child != nullptr) {
				m_children.push_back(std::move(child));
			}
		}
	}
}

ska::ASTNode::ASTNode(Operator o, Token identifierToken) :
	m_op(std::move(o)),
	m_token(std::move(identifierToken)) {
}

const std::optional<ska::Type>& ska::ASTNode::type() const {
	return m_type;
}

void ska::ASTNode::linkSymbol(ScopedSymbolTable& symbolTable) {
	m_symbolTable = &symbolTable;
	assert(m_symbolTable->symbol() != nullptr);
	refreshSymbolType();
	SLOG(ska::LogLevel::Info) << "Linking symbol \"" << *m_symbolTable->symbol() << "\" in node \"" << m_token << "\"";
}

void ska::ASTNode::refreshSymbolType() {
	auto* sym = symbol();
	if (sym != nullptr && m_type.has_value()) {
		SLOG(ska::LogLevel::Debug) << "Current symbol \"" << sym->name() << "\" has type \"" << sym->type() << "\"";
		if (sym->changeTypeIfRequired(m_type.value())) {
			SLOG(ska::LogLevel::Debug) << "Symbol \"" << sym->name() << "\" in node \"" << m_token << "\" has type updated \"" << sym->type() << "\"";
		}
	}
}

const ska::Symbol* ska::ASTNode::symbol() const {
	return m_symbolTable == nullptr ? nullptr : m_symbolTable->symbol();
}

ska::Symbol* ska::ASTNode::symbol() {
	return m_symbolTable == nullptr ? nullptr : m_symbolTable->symbol();
}

bool ska::ASTNode::updateType(Type type) {
	m_type = std::move(type);
	refreshSymbolType();
	return true;
}

bool ska::ASTNode::isSymbolicLeaf() const {
	return m_symbolTable != nullptr && m_children.size() < 2;
}

void ska::ASTNode::prettyPrint(std::ostream& os, int depth, const ScopedSymbolTable* parentNodeSymbolTable) const {
	const auto& node = *this;
	// Compute the change of symbol table in current level to represent the symbol table directly inside the ast pretty-printing
	const auto changedScope = node.symbolTable() != nullptr && parentNodeSymbolTable != nullptr && node.symbolTable() != parentNodeSymbolTable;
	// Print the current node with indentation based on its depth
	for (int i = 0; i < depth - (changedScope ? 1 : 0); i++) {
		os << "| ";
	}
	// If scope changed, print a '+' as last char instead of the classic '|'
	if (changedScope) {
		os << "+ ";
	}

	os << node.m_op << " - ";
	if (node.symbol() != nullptr) {
		os << *node.symbol();
	} else {
		if (node.m_type.has_value()) {
			os << node.m_type.value() << " ";
		}
		os << "\"" << node.m_token << "\"";
	}

	os << std::endl;
	// Recursively print each child node, increasing the depth
	for (const auto& child : node.m_children) {
		child->prettyPrint(os, depth + 1, node.symbolTable());
	}
}

std::ostream& ska::operator<<(std::ostream& stream, const ASTNode& node) {
	node.prettyPrint(stream);
	return stream;
}
