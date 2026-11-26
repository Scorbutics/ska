#pragma once
#include <vector>
#include <deque>
#include <variant>
#include <memory>
#include <optional>
#include "Token.h"
#include "ASTNodePtr.h"
#include "Operator.h"
#include "ExpressionType.h"
#include "Service/TypeBuilder/TypeBuildersContainer.h"
#include "Type.h"

namespace ska {
	struct Cursor;
	class Symbol;
	class StatementParser;
	class ASTFactory;
	class ScriptAST;

	class ASTNode {
	public:
		ASTNode(ASTNode&&) noexcept = default;
		ASTNode(const ASTNode&) = delete;
		~ASTNode() = default;

		bool has(const Token& t) const { return m_token == t; }

		bool logicalEmpty() const { return m_token.type() == TokenType::EMPTY && m_op == Operator::UNARY; }

		std::string name() const { return m_token.name(); }
		std::size_t size() const { return m_children.size(); }

		const Cursor& positionInScript() const { return m_token.position(); }

		TokenType tokenType() const { return m_token.type(); }

		auto& operator[](const std::size_t index) { assert(index < m_children.size()); return *m_children[index]; }
		const auto& operator[](const std::size_t index) const { assert(index < m_children.size()); return *m_children[index]; }

		const auto begin() const { return std::begin(m_children); }
		const auto end() const { return std::end(m_children); }

		const auto rbegin() const { return std::rbegin(m_children); }
		const auto rend() const { return std::rend(m_children); }

		auto begin() { return std::begin(m_children); }
		auto end() { return std::end(m_children); }

		auto rbegin() { return std::rbegin(m_children); }
		auto rend() { return std::rend(m_children); }

		const auto& op() const { return m_op; }

		void linkSymbol(ScopedSymbolTable& symbolTable);
		const std::optional<Type>& type() const;

		bool updateType(Type type);

		const Symbol* symbol() const;
		Symbol* symbol();

		const auto* symbolTable() const { return m_symbolTable; }
		auto* symbolTable() { return m_symbolTable; }

		bool isSymbolicLeaf() const;

	private:
		friend class ASTFactory;
		ASTNode();

    	explicit ASTNode(Token t, ASTNodePtr l = nullptr, ASTNodePtr r = nullptr);

    	ASTNode(Operator o, Token identifierToken = Token{}, std::vector<ASTNodePtr> children = std::vector<ASTNodePtr>{});
		ASTNode(Operator o, Token identifierToken, std::deque<ASTNodePtr> children);
    	ASTNode(Operator o, Token identifierToken = Token{});

		void refreshSymbolType();

		Operator m_op = Operator::UNARY;
		std::optional<Type> m_type;
		ScopedSymbolTable* m_symbolTable = nullptr;

		Token m_token;
		std::vector<ASTNodePtr> m_children;

		friend std::ostream& operator<<(std::ostream& stream, const ASTNode& node);
		void prettyPrint(std::ostream& os, int depth = 0, const ScopedSymbolTable* parentNodeSymbolTable = nullptr) const;
	};

}

