#pragma once

namespace ska {
	class ASTNode;

	class ForTokenEvent {
	public:
		ForTokenEvent(ASTNode& node) : node(node) {}

		auto& rootNode() {
			return node;
		}

		const auto& rootNode() const {
			return node;
		}

	private:
    	ASTNode& node;
	};
}
