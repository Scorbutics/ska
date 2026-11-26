#pragma once

namespace ska {
		class ASTNode;
	
	enum class BlockTokenEventType {
		START,
		END
	};
		
	struct BlockTokenEvent {
		BlockTokenEvent(ASTNode& n, BlockTokenEventType t) : 
			node(n), m_type(t) {
		}

		const auto& type() const {
			return m_type;
		}

		auto& rootNode() {
			return node;
		}

		const auto& rootNode() const {
			return node;
		}

	private:
			ASTNode& node;
			BlockTokenEventType m_type;
		};
}
