#pragma once
#include "../../external/gsl/gsl/pointers"

namespace ska {
	struct Node {
		class CompareDist {
		public:
			bool operator()(Node& n1, Node& n2) const {
				return n1.m_priority > n2.m_priority;
			}
		};

		Node(int column, int line, bool walkable);

		Node(Node&& n) noexcept : 
			column(n.column), 
			line(n.line), 
			walkable(n.walkable),
			m_notInPath(n.m_notInPath), 
			m_costSoFar(n.m_costSoFar),
			m_heuristic(n.m_heuristic),
			m_priority(n.m_priority) {
			if(n.m_parent != &n) {
				m_parent = n.m_parent;
			}
		}

		Node(const Node& n) = delete;

		Node& operator=(const Node& n) = delete;
		Node& operator=(Node&& n) = delete;

		void calculateGlobalCost(const Node& parent, const Node& nodeGoal);

		const Node& parent() const {
			return *m_parent;
		}

		bool isNotAlreadyInPath() const{
			return m_notInPath;
		}

		bool willHaveLessCost() const {
			return m_parent->m_costSoFar + 1 < m_costSoFar;
		}

		const int column;
		const int line;
		const bool walkable;

	private:
		gsl::not_null<const Node*> m_parent = this;

		bool m_notInPath = true;
		int m_costSoFar = 0;
		int m_heuristic = 0;
		int m_priority = 0;
	};

	inline bool operator==(const Node& n1, const Node& n2) {
		return n1.column == n2.column && n1.line == n2.line;
	}

	inline bool operator!=(const Node& n1, const Node& n2) {
		return !operator==(n1, n2);
	}

}
