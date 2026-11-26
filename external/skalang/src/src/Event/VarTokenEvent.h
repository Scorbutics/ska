#pragma once

#include "NodeValue/Type.h"

namespace ska {
	class ASTNode;
	class ScriptAST;

	enum class VarTokenEventType {
		FUNCTION_DECLARATION,
		PARAMETER_DECLARATION,
		DECLARATION,
		AFFECTATION,
		USE
	};

	class VarTokenEvent {
	public:
		static VarTokenEvent MakeFunction(ASTNode& node, ScriptAST& s) {
			return VarTokenEvent{ node, s, VarTokenEventType::FUNCTION_DECLARATION };
		}

		static VarTokenEvent MakeParameter(ASTNode& node, ASTNode& typeNode, ScriptAST& s) {
			return VarTokenEvent { node, s, typeNode, VarTokenEventType::PARAMETER_DECLARATION };
		}

		static VarTokenEvent MakeUse(ASTNode& node, ScriptAST& s) {
			return VarTokenEvent{ node, s, VarTokenEventType::USE };
		}

		template <VarTokenEventType type>
		static VarTokenEvent Make(ASTNode& node, ScriptAST& s) {
			static_assert(type != VarTokenEventType::PARAMETER_DECLARATION && type != VarTokenEventType::FUNCTION_DECLARATION);
			return VarTokenEvent { node, s, type };
		}

		auto& rootNode() {
			return m_node;
		}

		const auto& rootNode() const {
			return m_node;
		}

		auto& typeNode() {
			assert(m_typeNode != nullptr);
			return *m_typeNode;
		}

		const auto& typeNode() const {
			assert(m_typeNode != nullptr);
			return *m_typeNode;
		}

		const auto& type() const {
			return m_type;
		}

		auto& script() {
			return m_script;
		}

		const auto& script() const {
			return m_script;
		}

		const ASTNode& var() const;
		const ASTNode* val() const;

		const std::optional<Type>& varType() const;
		std::optional<Type> valType() const;

		std::string value() const;
		std::string name() const;

	private:
		VarTokenEvent(ASTNode& node, ScriptAST& s, ASTNode& typeNode, VarTokenEventType type) : m_node(node), m_typeNode(&typeNode), m_type(type), m_script(s) {}
		VarTokenEvent(ASTNode& node, ScriptAST& s, VarTokenEventType type = VarTokenEventType::DECLARATION) : m_node(node), m_type(type), m_script(s) {}

		ASTNode& m_node;
		ASTNode* m_typeNode = nullptr;
		VarTokenEventType m_type;
		ScriptAST& m_script;
	};
}
