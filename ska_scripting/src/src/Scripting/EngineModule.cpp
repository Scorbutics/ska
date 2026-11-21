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
	const std::string& StringParameter(const ska::NodeValue& value) {
		return *value.nodeval<ska::StringShared>();
	}
}

ska::scripting::EngineModule::EngineModule(ScriptVM& vm, EntityManager& entityManager, const EntityLocator& locator, const EntityHandles& handles, MemoryScript* savegame) :
	lang::Module<bytecode::Interpreter> { vm.modules(), "ska.engine", "std:ska.engine" },
	m_vm(vm),
	m_entityManager(entityManager),
	m_handles(handles),
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
		return NodeValue{ m_handles.handle(m_vm.context().origin) };
	});

	m_builder.bindField("entity", [this, &locator](std::vector<NodeValue> params) {
		const auto* entity = locator.getEntityId(StringParameter(params[0]));
		return NodeValue{ entity == nullptr ? -1L : m_handles.handle(*entity) };
	});

	m_builder.bindField("exists", [this](std::vector<NodeValue> params) {
		return NodeValue{ m_handles.find(params[0].nodeval<long>()).has_value() };
	});

	m_builder.bindField("on", [this](std::vector<NodeValue> params) {
		return NodeValue{ static_cast<long>(m_vm.addHook(StringParameter(params[0]), params[1])) };
	});

	m_builder.bindField("removeHook", [this](std::vector<NodeValue> params) {
		const auto hook = params[0].nodeval<long>();
		if (hook > 0) {
			m_vm.removeHook(static_cast<std::size_t>(hook));
		}
		return NodeValue{};
	});

	m_builder.bindField("disableComponent", [this](std::vector<NodeValue> params) {
		component(StringParameter(params[1])).disable(m_handles.entity(params[0].nodeval<long>()));
		return NodeValue{};
	});

	m_builder.bindField("enableComponent", [this](std::vector<NodeValue> params) {
		component(StringParameter(params[1])).enable(m_handles.entity(params[0].nodeval<long>()));
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

std::size_t ska::scripting::EngineModule::emit(const std::string& event, const std::optional<EntityId>& entity) {
	auto context = ScriptContext{};
	context.origin = entity;
	return m_vm.emit(event, { NodeValue{ m_handles.handle(entity) } }, std::move(context));
}

const ska::scripting::EngineModule::ComponentAccess& ska::scripting::EngineModule::component(const std::string& name) const {
	auto access = m_components.find(name);
	if (access == m_components.end()) {
		throw std::runtime_error("the component \"" + name + "\" cannot be disabled by the scripts (not registered)");
	}
	return access->second;
}
