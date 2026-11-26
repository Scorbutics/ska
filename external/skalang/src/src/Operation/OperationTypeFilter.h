#pragma once

#include "OperationType.h"
#include "NodeValue/AST.h"

namespace ska {
	template<>
	class OperationType<Operator::FILTER> {
	private:
		const ASTNode& node;
	public:
		OperationType(const ASTNode& node) : node(node) {
			assert(node.size() == 3);
			assert(node[1].size() == 2);
		}

		inline auto& GetStatement() const {
			return node[2];
		}

		inline auto& GetCollection() const {
			return node[0];
		}

		inline auto& GetCollectionIterator() const {
			return node[1][0];
		}

		inline auto& GetCollectionIteratorIndex() const {
			return node[1][1];
		}
	};
}
