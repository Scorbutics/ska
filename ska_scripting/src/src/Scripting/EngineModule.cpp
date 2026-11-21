#include "__internalConfig/LoggerConfig.h"
#include "EngineModule.h"
#include "ScriptVM.h"
#include "Core/ECS/EntityLocator.h"
#include "Core/Data/MemoryScript.h"
#include "Core/ECS/Basics/Input/InputComponent.h"

namespace ska {
	namespace scripting {
		struct EngineModuleLog;
	}
}
SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::scripting::EngineModuleLog);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::scripting::EngineModuleLog)

namespace {
	long EntityValue(const std::optional<ska::EntityId>& entity) {
		return entity.has_value() ? static_cast<long>(*entity) : -1L;
	}

	ska::EntityId EntityParameter(const ska::NodeValue& value) {
		const auto entity = value.nodeval<long>();
		if (entity < 0) {
			throw std::runtime_error("invalid entity : " + std::to_string(entity));
		}
		return static_cast<ska::EntityId>(entity);
	}

	const std::string& StringParameter(const ska::NodeValue& value) {
		return *value.nodeval<ska::StringShared>();
	}
}

ska::scripting::EngineModule::EngineModule(ScriptVM& vm, EntityManager& entityManager, const EntityLocator& locator, MemoryScript* savegame) :
	lang::Module<bytecode::Interpreter> { vm.modules(), "ska.engine", "std:ska.engine" },
	m_vm(vm),
	m_entityManager(entityManager),
	m_random(std::random_device{}()),
	m_builder(BridgeBuilder<bytecode::Interpreter> { m_bridge }) {

	registerComponent<InputComponent>("InputComponent");

	m_builder.bindField("wait", [this](std::vector<NodeValue> params) {
		const auto duration = params[0].nodeval<long>();
		m_vm.wait(duration > 0 ? static_cast<unsigned int>(duration) : 0u);
		return NodeValue{};
	});

	m_builder.bindField("log", [this](std::vector<NodeValue> params) {
		LOG_INFO << "[" << m_vm.context().name << "] " << StringParameter(params[0]);
		return NodeValue{};
	});

	m_builder.bindField("random", [this](std::vector<NodeValue> params) {
		const auto max = params[0].nodeval<long>();
		if (max <= 0) {
			return NodeValue{ 0L };
		}
		return NodeValue{ std::uniform_int_distribution<long>{ 0, max - 1 }(m_random) };
	});

	m_builder.bindField("origin", [this](std::vector<NodeValue>) {
		return NodeValue{ EntityValue(m_vm.context().origin) };
	});

	m_builder.bindField("entity", [&locator](std::vector<NodeValue> params) {
		const auto* entity = locator.getEntityId(StringParameter(params[0]));
		return NodeValue{ entity == nullptr ? -1L : static_cast<long>(*entity) };
	});

	m_builder.bindField("disableComponent", [this](std::vector<NodeValue> params) {
		component(StringParameter(params[1])).disable(EntityParameter(params[0]));
		return NodeValue{};
	});

	m_builder.bindField("enableComponent", [this](std::vector<NodeValue> params) {
		component(StringParameter(params[1])).enable(EntityParameter(params[0]));
		return NodeValue{};
	});

	m_builder.bindField("variable", [savegame](std::vector<NodeValue> params) {
		return NodeValue{ savegame == nullptr ? 0L : static_cast<long>(savegame->getGameVariable(StringParameter(params[0]))) };
	});

	m_builder.bindField("setVariable", [savegame](std::vector<NodeValue> params) {
		if (savegame != nullptr) {
			savegame->setGameVariable(StringParameter(params[0]), static_cast<int>(params[1].nodeval<long>()));
		}
		return NodeValue{};
	});

	m_builder.bindField("script", [this](std::vector<NodeValue> params) {
		auto arguments = std::vector<std::string>{};
		const auto& values = params[1].nodeval<NodeValueArray>();
		if (values != nullptr) {
			for (const auto& value : *values) {
				arguments.push_back(value.convertString());
			}
		}
		auto context = ScriptContext{};
		context.origin = m_vm.context().origin;
		m_vm.start(StringParameter(params[0]), std::move(arguments), std::move(context));
		return NodeValue{};
	});

	m_builder.generate();
}

const ska::scripting::EngineModule::ComponentAccess& ska::scripting::EngineModule::component(const std::string& name) const {
	auto access = m_components.find(name);
	if (access == m_components.end()) {
		throw std::runtime_error("the component \"" + name + "\" cannot be disabled by the scripts (not registered)");
	}
	return access->second;
}
