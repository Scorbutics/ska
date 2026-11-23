#include "__internalConfig/LoggerConfig.h"
#include <memory>
#include "DevModule.h"
#include "ScriptVM.h"
#include "Core/Dev/DevConsole.h"

namespace {
	const std::string& StringParameter(const ska::NodeValue& value) {
		return *value.nodeval<ska::StringShared>();
	}

	// The answer of a console verb, given later (at the end of the frame, after some logic steps)
	struct Pending {
		bool done = false;
		ska::DevReply reply;
	};
}

ska::scripting::DevModule::DevModule(ScriptVM& vm) :
	lang::Module<bytecode::Interpreter> { vm.modules(), "ska.dev", "std:ska.dev" },
	m_vm(vm),
	m_builder(BridgeBuilder<bytecode::Interpreter> { m_bridge }) {

	m_builder.bindField("reply", [this](std::vector<NodeValue> params) {
		const auto& reply = m_vm.context().reply;
		if (reply != nullptr) {
			if (!reply->empty()) {
				*reply += "\n";
			}
			*reply += StringParameter(params[0]);
		}
		return NodeValue{};
	});

	m_builder.bindField("steps", [](std::vector<NodeValue>) {
		return NodeValue{ static_cast<long>(DevConsole::get().steps()) };
	});

	m_builder.bindField("states", [](std::vector<NodeValue>) {
		return NodeValue{ std::make_shared<std::string>(DevConsole::get().states()) };
	});

	m_builder.bindField("shot", [this](std::vector<NodeValue> params) {
		auto& console = DevConsole::get();
		if (!console.opened()) {
			return NodeValue{ std::make_shared<std::string>() };
		}
		auto pending = std::make_shared<Pending>();
		console.shot(StringParameter(params[0]), [pending](DevReply reply) {
			pending->done = true;
			pending->reply = std::move(reply);
		});
		m_vm.suspendUntil([pending]() { return pending->done; }, [pending]() {
			return NodeValue{ std::make_shared<std::string>(pending->reply.ok ? pending->reply.value : std::string{}) };
		});
		return NodeValue{};
	});

	m_builder.bindField("press", [this](std::vector<NodeValue> params) {
		const auto& key = StringParameter(params[0]);
		const auto steps = params[1].nodeval<long>();
		auto pending = std::make_shared<Pending>();
		const auto known = DevConsole::get().press(key, static_cast<unsigned int>(steps > 0 ? steps : 1), [pending](DevReply reply) {
			pending->done = true;
			pending->reply = std::move(reply);
		});
		if (!known) {
			throw std::runtime_error("Dev.press : unknown key \"" + key + "\" (SDL key names : Return, Space, Escape, Up, Left, A, 1...)");
		}
		m_vm.suspendUntil([pending]() { return pending->done; });
		return NodeValue{};
	});

	m_builder.bindField("pause", [](std::vector<NodeValue>) {
		DevConsole::get().pause();
		return NodeValue{};
	});

	m_builder.bindField("resume", [](std::vector<NodeValue>) {
		DevConsole::get().resume();
		return NodeValue{};
	});

	m_builder.bindField("paused", [](std::vector<NodeValue>) {
		return NodeValue{ DevConsole::get().paused() };
	});

	m_builder.bindField("step", [this](std::vector<NodeValue> params) {
		auto& console = DevConsole::get();
		const auto steps = params[0].nodeval<long>();
		if (steps <= 0) {
			return NodeValue{};
		}
		const auto target = console.steps() + static_cast<unsigned long>(steps);
		if (console.paused()) {
			console.step(static_cast<unsigned int>(steps));
		}
		// Paused or not : the script goes on once the steps are done
		m_vm.suspendUntil([target]() { return DevConsole::get().steps() >= target; });
		return NodeValue{};
	});

	m_builder.generate();
}
