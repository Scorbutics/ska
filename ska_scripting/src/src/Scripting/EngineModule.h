#pragma once
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>

#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"
#include "Core/ECS/EntityManager.h"

namespace ska {
	class EntityLocator;
	class MemoryScript;

	namespace scripting {
		class ScriptVM;

		// Engine functions for the scripts : Engine = import "bind:ska.engine" (see the "ska.engine" script template)
		class EngineModule : public lang::Module<bytecode::Interpreter> {
		public:
			// "savegame" : storage of the game variables (optional)
			EngineModule(ScriptVM& vm, EntityManager& entityManager, const EntityLocator& locator, MemoryScript* savegame = nullptr);
			~EngineModule() override = default;

			// Makes a component type removable by the scripts (disableComponent / enableComponent), under a name
			template <class Component>
			void registerComponent(const std::string& name) {
				auto removed = std::make_shared<std::unordered_map<EntityId, Component>>();
				auto& entityManager = m_entityManager;
				m_components[name] = ComponentAccess {
					[&entityManager, removed](EntityId entity) {
						if (entityManager.hasComponent<Component>(entity)) {
							(*removed)[entity] = entityManager.getComponent<Component>(entity);
							entityManager.removeComponent<Component>(entity);
						}
					},
					[&entityManager, removed](EntityId entity) {
						auto component = removed->find(entity);
						if (component != removed->end()) {
							entityManager.addComponent<Component>(entity, std::move(component->second));
							removed->erase(component);
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
			std::unordered_map<std::string, ComponentAccess> m_components;
			std::mt19937 m_random;
			BridgeBuilder<bytecode::Interpreter> m_builder;
		};
	}
}
