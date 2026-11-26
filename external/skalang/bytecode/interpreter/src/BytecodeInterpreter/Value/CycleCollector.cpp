#include <unordered_map>
#include "Config/LoggerConfigLang.h"
#include "CycleCollector.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::CycleCollector);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::CycleCollector)

namespace {
	enum class ObjectKind { Cell, Upvalues, Array, Map };

	struct TrackedObject {
		ObjectKind kind;
		// Strong reference held during the collection (counted in the reference count : removed from "externalReferences")
		std::shared_ptr<void> object;
		long externalReferences = 0;
		bool reachable = false;
	};

	struct ObjectGraph {
		std::vector<TrackedObject> objects;
		std::unordered_map<const void*, std::size_t> indexes;

		template <class T>
		void add(ObjectKind kind, std::vector<std::weak_ptr<T>>& registry) {
			auto alive = std::vector<std::weak_ptr<T>>{};
			alive.reserve(registry.size());
			for (auto& weak : registry) {
				auto strong = weak.lock();
				if (strong == nullptr) {
					// Already freed by reference counting
					continue;
				}
				alive.push_back(weak);
				if (indexes.find(strong.get()) != indexes.end()) {
					continue;
				}
				// The strong reference held here is not an external one
				const auto references = static_cast<long>(strong.use_count()) - 1;
				indexes.emplace(strong.get(), objects.size());
				objects.push_back(TrackedObject{ kind, std::move(strong), references });
			}
			registry = std::move(alive);
		}

		TrackedObject* find(const void* object) {
			auto it = indexes.find(object);
			return it == indexes.end() ? nullptr : &objects[it->second];
		}

		// Calls "visitor" with each registered object directly referenced by "object" (once per strong reference)
		template <class Visitor>
		void visitChildren(const TrackedObject& object, Visitor&& visitor) {
			const auto visitValue = [&](const ska::NodeValue& value) {
				value.visitReferences([&](const auto& reference) {
					if (auto* child = find(reference.get())) {
						visitor(*child);
					}
				});
			};

			switch (object.kind) {
			case ObjectKind::Cell:
				visitValue(*static_cast<const ska::NodeValue*>(object.object.get()));
				break;
			case ObjectKind::Upvalues:
				for (const auto& cell : *static_cast<const ska::NodeValue::Upvalues*>(object.object.get())) {
					if (cell != nullptr) {
						if (auto* child = find(cell.get())) {
							visitor(*child);
						}
					}
				}
				break;
			case ObjectKind::Array:
				for (const auto& value : *static_cast<const ska::NodeValueArrayRaw*>(object.object.get())) {
					visitValue(value);
				}
				break;
			case ObjectKind::Map:
				for (const auto& entry : *static_cast<const ska::NodeValueMap::element_type*>(object.object.get())) {
					visitValue(entry.second);
				}
				break;
			}
		}
	};
}

void ska::bytecode::CycleCollector::track(const Cell& cell) {
	m_cells.push_back(cell);
	m_trackedSinceLastCollection++;
}

void ska::bytecode::CycleCollector::track(const UpvaluesPtr& upvalues) {
	m_upvalues.push_back(upvalues);
	m_trackedSinceLastCollection++;
}

void ska::bytecode::CycleCollector::track(const NodeValueArray& array) {
	m_arrays.push_back(array);
	m_trackedSinceLastCollection++;
}

void ska::bytecode::CycleCollector::track(const NodeValueMap& map) {
	m_maps.push_back(map);
	m_trackedSinceLastCollection++;
}

void ska::bytecode::CycleCollector::collectIfNeeded() {
	if (m_trackedSinceLastCollection >= CollectionThreshold) {
		collect();
	}
}

std::size_t ska::bytecode::CycleCollector::collect() {
	m_trackedSinceLastCollection = 0;

	auto graph = ObjectGraph{};
	graph.add(ObjectKind::Cell, m_cells);
	graph.add(ObjectKind::Upvalues, m_upvalues);
	graph.add(ObjectKind::Array, m_arrays);
	graph.add(ObjectKind::Map, m_maps);

	// 1. Remove the references held by registered objects
	for (const auto& object : graph.objects) {
		graph.visitChildren(object, [](TrackedObject& child) {
			child.externalReferences--;
		});
	}

	// 2. Objects still referenced from outside are alive, as everything reachable from them
	auto toVisit = std::vector<TrackedObject*>{};
	for (auto& object : graph.objects) {
		if (object.externalReferences > 0) {
			object.reachable = true;
			toVisit.push_back(&object);
		}
	}
	while (!toVisit.empty()) {
		auto* object = toVisit.back();
		toVisit.pop_back();
		graph.visitChildren(*object, [&toVisit](TrackedObject& child) {
			if (!child.reachable) {
				child.reachable = true;
				toVisit.push_back(&child);
			}
		});
	}

	// 3. Clear the garbage objects content (the strong references held by the graph keep them valid until the end)
	std::size_t garbage = 0;
	for (auto& object : graph.objects) {
		if (object.reachable) {
			continue;
		}
		garbage++;
		switch (object.kind) {
		case ObjectKind::Cell:
			*static_cast<NodeValue*>(object.object.get()) = NodeValue{};
			break;
		case ObjectKind::Upvalues:
			static_cast<NodeValue::Upvalues*>(object.object.get())->clear();
			break;
		case ObjectKind::Array:
			static_cast<NodeValueArrayRaw*>(object.object.get())->clear();
			break;
		case ObjectKind::Map:
			static_cast<NodeValueMap::element_type*>(object.object.get())->clear();
			break;
		}
	}

	LOG_INFO << "Cycle collection : " << garbage << " garbage objects found among " << graph.objects.size();
	return garbage;
}
