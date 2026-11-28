#pragma once
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Core/ECS/EntityManager.h"
#include "EntityHandles.h"

namespace ska {
	class EntityLocator;
	class MemoryScript;

	namespace scripting {
		class ScriptVM;

		// Engine functions for the scripts : Engine = import "bind:ska.engine" (see the "ska.engine" script template)
		class EngineModule : public lang::Module<bytecode::Interpreter> {
		public:
			// "handles" : the entities given to the scripts. "savegame" : storage of the game variables (optional)
			EngineModule(ScriptVM& vm, EntityManager& entityManager, const EntityLocator& locator, const EntityHandles& handles, MemoryScript* savegame = nullptr);
			~EngineModule() override = default;

			// Starts the hooks of an event (declared with ScriptVM::declareEvent) about an entity (hook : function(entity: int))
			std::size_t emit(const std::string& event, const std::optional<EntityId>& entity);

			// Makes a component type removable by the scripts (disableComponent / enableComponent), under a name : the script
			// holds it (EntityManager::hold : the game can hold it too, it comes back when every owner released it). A script
			// disables it once (a second disable does nothing), its enable releases it
			template <class Component>
			void registerComponent(const std::string& name) {
				auto disabled = std::make_shared<std::unordered_set<EntityId>>();
				auto& entityManager = m_entityManager;
				m_components[name] = ComponentAccess {
					[&entityManager, disabled](EntityId entity) {
						if (disabled->insert(entity).second) {
							entityManager.hold<Component>(entity);
						}
					},
					[&entityManager, disabled](EntityId entity) {
						if (disabled->erase(entity) > 0) {
							entityManager.release<Component>(entity);
						}
					}
				};
			}

		private:
			struct ComponentAccess {
				std::function<void(EntityId)> disable;
				std::function<void(EntityId)> enable;
			};
			const ComponentAccess& component(const std::string& name) const;

			ScriptVM& m_vm;
			EntityManager& m_entityManager;
			const EntityHandles& m_handles;
			std::unordered_map<std::string, ComponentAccess> m_components;
			std::mt19937 m_random;
			BridgeBuilder<bytecode::Interpreter> m_builder;
		};
	}
}
