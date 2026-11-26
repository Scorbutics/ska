#pragma once
#include <cassert>

namespace ska {
	class ASTNode;
	class ScriptAST;

	enum class ReturnTokenEventType {
    	START,
    	BUILTIN,
    	OBJECT
	};

	class ReturnTokenEvent {
	public:
    	ReturnTokenEvent(ScriptAST& s) : m_type(ReturnTokenEventType::START), m_script(s) {}

    	template<ReturnTokenEventType type>
    	static ReturnTokenEvent Make(ASTNode& node, ScriptAST& s) {
        	static_assert(type != ReturnTokenEventType::START);
        	return ReturnTokenEvent { node, type, s };
    	}

    	auto& rootNode() {
			assert(m_node != nullptr);
        	return *m_node;
		}

		const auto& rootNode() const {
			assert(m_node != nullptr);
        	return *m_node;
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

	private:
    	ReturnTokenEvent(ASTNode& node, ReturnTokenEventType type, ScriptAST& s) : m_node(&node), m_type(type), m_script(s) {
    	}

		ASTNode* m_node = nullptr;
    	ReturnTokenEventType m_type;
		ScriptAST& m_script;
	};
}
