#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include "Runtime/Value/NodeValue.h"

namespace ska {
	namespace bytecode {

		// Collects the reference cycles that reference counting alone cannot free
		// (ex : two closures capturing each other through cells, an object stored in a variable captured by one of its methods).
		//
		// Heap objects able to hold references (cells, lists of captured cells, arrays / objects) are registered when created.
		// A collection works as the CPython cycle collector :
		// 1. the references held by registered objects to other registered objects are subtracted from their reference counts.
		// 2. an object still having references is referenced from outside the registered objects (a call frame, the value stack,
		//    a script variable, the host program...) : it is alive, as every object reachable from it.
		// 3. the other objects are only reachable through cycles of garbage : their content is cleared, which breaks the cycles,
		//    and reference counting frees them.
		// Unknown references (from objects that are not registered) are counted as external ones :
		// they can only keep objects alive, never free a live one.
		class CycleCollector {
		public:
			using Cell = NodeValue::Cell;
			using UpvaluesPtr = std::shared_ptr<NodeValue::Upvalues>;

			void track(const Cell& cell);
			void track(const UpvaluesPtr& upvalues);
			void track(const NodeValueArray& array);
			void track(const NodeValueMap& map);

			// Runs a collection if enough objects were registered since the last one
			void collectIfNeeded();

			// Runs a collection. Returns the number of garbage objects found (and cleared).
			std::size_t collect();

			std::size_t trackedObjects() const { return m_cells.size() + m_upvalues.size() + m_arrays.size() + m_maps.size(); }

		private:
			static constexpr std::size_t CollectionThreshold = 1000;

			std::vector<std::weak_ptr<NodeValue>> m_cells;
			std::vector<std::weak_ptr<NodeValue::Upvalues>> m_upvalues;
			std::vector<std::weak_ptr<NodeValueArrayRaw>> m_arrays;
			std::vector<std::weak_ptr<NodeValueMap::element_type>> m_maps;
			std::size_t m_trackedSinceLastCollection = 0;
		};
	}
}
