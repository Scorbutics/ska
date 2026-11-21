#pragma once

#include <cassert>

namespace ska {
	class ASTNode;
	class ScriptAST;

	enum class FilterTokenEventType {
		INIT_COLLECTION,
		DECLARATION,
		DEFINITION
	};
		
	struct FilterTokenEvent {
		FilterTokenEvent(ASTNode& n, FilterTokenEventType t, ScriptAST& s) : 
			node(n), m_type(t), m_script(s) {
		}

		auto& rootNode() {
			return node;
		}

		auto& script() {
			return m_script;
		}

		const auto& type() const {
			return m_type;
		}

		const auto& elementIterator() const {
			assert(m_type != FilterTokenEventType::INIT_COLLECTION);
			return node[0];
		}

		const auto& indexIterator() const {
			assert(m_type != FilterTokenEventType::INIT_COLLECTION);
			return node[1];
		}

		auto& elementIterator() {
			assert(m_type != FilterTokenEventType::INIT_COLLECTION);
			return node[0];
		}

		auto& indexIterator() {
			assert(m_type != FilterTokenEventType::INIT_COLLECTION);
			return node[1];
		}

	private:
		ASTNode& node;
		ScriptAST& m_script;
		FilterTokenEventType m_type;
	};
}
