#pragma once
#include <memory>
#include <string>

namespace ska {
	class ScriptComponent;
	class ScriptAutoSystem;
	class EntityManager;

	class Command {
	public:
		explicit Command(EntityManager& entityManager) : m_entityManager(entityManager) {}
		void operator=(const Command&) = delete;
		virtual std::string process(ScriptAutoSystem& system, ScriptComponent& script, std::stringstream& streamCmd) = 0;
		virtual ~Command() = default;

	protected:
		EntityManager& m_entityManager;

	};
	using CommandPtr = std::unique_ptr<Command>;
}
