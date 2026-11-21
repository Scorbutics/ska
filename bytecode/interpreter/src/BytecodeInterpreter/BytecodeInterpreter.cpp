#include "Config/LoggerConfigLang.h"
#include "BytecodeInterpreter.h"

#include "NodeValue/AST.h"
#include "NodeValue/Symbol.h"
#include "BytecodeScript.h"
#include "Generator/Value/BytecodeGenerationOutput.h"
#include "Generator/BytecodeGenerator.h"

#include "Units/InterpreterCommandMov.h"
#include "Units/InterpreterCommandAddI.h"
#include "Units/InterpreterCommandSubI.h"
#include "Units/InterpreterCommandMulI.h"
#include "Units/InterpreterCommandDivI.h"
#include "Units/InterpreterCommandAddD.h"
#include "Units/InterpreterCommandSubD.h"
#include "Units/InterpreterCommandMulD.h"
#include "Units/InterpreterCommandDivD.h"
#include "Units/InterpreterCommandAddStr.h"
#include "Units/InterpreterCommandPushArrArr.h"
#include "Units/InterpreterCommandSubArr.h"
#include "Units/InterpreterCommandCmpStr.h"
#include "Units/InterpreterCommandCmpBool.h"
#include "Units/InterpreterCommandCmpArr.h"
#include "Units/InterpreterCommandTestEq.h"
#include "Units/InterpreterCommandTestG.h"
#include "Units/InterpreterCommandTestGe.h"
#include "Units/InterpreterCommandTestL.h"
#include "Units/InterpreterCommandTestLe.h"
#include "Units/InterpreterCommandTestNeq.h"
#include "Units/InterpreterCommandPushFArr.h"
#include "Units/InterpreterCommandPushBArr.h"
#include "Units/InterpreterCommandRet.h"
#include "Units/InterpreterCommandEnd.h"
#include "Units/InterpreterCommandPush.h"
#include "Units/InterpreterCommandPop.h"
#include "Units/InterpreterCommandPopInArr.h"
#include "Units/InterpreterCommandPopInVar.h"
#include "Units/InterpreterCommandJumpAbs.h"
#include "Units/InterpreterCommandJumpBind.h"
#include "Units/InterpreterCommandJumpRel.h"
#include "Units/InterpreterCommandConvID.h"
#include "Units/InterpreterCommandConvDI.h"
#include "Units/InterpreterCommandConvDStr.h"
#include "Units/InterpreterCommandConvIStr.h"
#include "Units/InterpreterCommandConvStrD.h"
#include "Units/InterpreterCommandConvStrI.h"
#include "Units/InterpreterCommandArrAccess.h"
#include "Units/InterpreterCommandArrLength.h"
#include "Units/InterpreterCommandScript.h"
#include "Units/InterpreterCommandJumpNif.h"
#include "Units/InterpreterCommandArrSet.h"
#include "Units/InterpreterCommandCell.h"
#include "Units/InterpreterCommandPushCell.h"
#include "InterpreterDeclarer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Interpreter);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Interpreter)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Interpreter)
#define LOG_WARN SLOG_STATIC(ska::LogLevel::Warn, ska::bytecode::Interpreter)

ska::bytecode::Interpreter::CommandInterpreter ska::bytecode::Interpreter::build(Generator& generator) {
	auto result = CommandInterpreter {};
	static constexpr auto maxCommandEnumIndex = static_cast<std::size_t>(Command::UNUSED_Last_Length);
	result.resize(maxCommandEnumIndex);

	InterpreterCommandDeclare<Command::MOV>(*this, result, generator);

	InterpreterCommandDeclare<Command::ADD_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::SUB_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::MUL_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::DIV_I>(*this, result, generator);

	InterpreterCommandDeclare<Command::ADD_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::SUB_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::MUL_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::DIV_D>(*this, result, generator);

	InterpreterCommandDeclare<Command::ADD_STR>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH_ARR_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH_F_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH_B_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::SUB_ARR>(*this, result, generator);

	InterpreterCommandDeclare<Command::CONV_D_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_I_D>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_I_STR>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_D_STR>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_STR_I>(*this, result, generator);
	InterpreterCommandDeclare<Command::CONV_STR_D>(*this, result, generator);

	InterpreterCommandDeclare<Command::CMP_STR>(*this, result, generator);
	InterpreterCommandDeclare<Command::CMP_BOOL>(*this, result, generator);
	InterpreterCommandDeclare<Command::CMP_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_EQ>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_G>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_GE>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_L>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_LE>(*this, result, generator);
	InterpreterCommandDeclare<Command::TEST_NEQ>(*this, result, generator);

	InterpreterCommandDeclare<Command::RET>(*this, result, generator);
	InterpreterCommandDeclare<Command::END>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH>(*this, result, generator);
	InterpreterCommandDeclare<Command::POP>(*this, result, generator);
	InterpreterCommandDeclare<Command::POP_IN_ARR>(*this, result, generator);
	InterpreterCommandDeclare<Command::POP_IN_VAR>(*this, result, generator);
	InterpreterCommandDeclare<Command::JUMP_ABS>(*this, result, generator);
	InterpreterCommandDeclare<Command::BIND>(*this, result, generator);
	InterpreterCommandDeclare<Command::JUMP_REL>(*this, result, generator);
	InterpreterCommandDeclare<Command::JUMP_NIF>(*this, result, generator);

	InterpreterCommandDeclare<Command::ARR_ACCESS>(*this, result, generator);
	InterpreterCommandDeclare<Command::ARR_LENGTH>(*this, result, generator);
	InterpreterCommandDeclare<Command::SCRIPT>(*this, result, generator);

	InterpreterCommandDeclare<Command::ARR_SET>(*this, result, generator);
	InterpreterCommandDeclare<Command::CELL>(*this, result, generator);
	InterpreterCommandDeclare<Command::PUSH_CELL>(*this, result, generator);

	return result;
}

ska::bytecode::Interpreter::Interpreter(StatementParser& parser, Generator& generator, const ReservedKeywordsPool& reserved) :
	m_parser(parser),
	m_generator(generator),
	m_commandInterpreter(build(generator)) {
}

void ska::bytecode::Interpreter::interpret(ExecutionContext& node) {
	if (!node.isGenerated(node.currentScriptId())) {
		LOG_WARN << "script \"" << node.currentScriptName() << "\" is not bytecode-generated yet... cannot interpret !";
		node.generate(m_parser, m_generator);
	}

	auto* script = node.currentScript();
	if (script == nullptr || script->topLevelExecuted()) {
		return;
	}

	// The script top level code runs in its own call frame (registers), left even if an error is thrown
	struct ScriptLevelFrame {
		Interpreter& interpreter;
		ExecutionContext& context;
		ScriptLevelFrame(Interpreter& interpreter, ExecutionContext& context) : interpreter(interpreter), context(context) {
			context.enterScriptLevel();
			if (interpreter.m_runningCoroutine != nullptr) { interpreter.m_nestedTopLevels++; }
		}
		~ScriptLevelFrame() {
			if (interpreter.m_runningCoroutine != nullptr) { interpreter.m_nestedTopLevels--; }
			context.leaveScriptLevel();
		}
	} scriptLevelFrame { *this, node };

	run(node, false);
	script->setTopLevelExecuted();
}

ska::bytecode::Interpreter::RunResult ska::bytecode::Interpreter::run(ExecutionContext& node, bool advance) {
	for(auto continueExecution = advance ? node.incInstruction() : !node.idle(); continueExecution; continueExecution = node.incInstruction()) {
		auto& instruction = node.currentInstruction();
		LOG_INFO << "[Script " << node.currentScriptId() << "] Interpreting " << instruction;
		auto& builder = m_commandInterpreter[static_cast<std::size_t>(instruction.command())];
		assert(builder != nullptr);
		auto nodeValue = builder->interpret(node);
		if (!nodeValue.empty()) {
			LOG_DEBUG << "Setting " << nodeValue.convertString() << " for " << instruction.dest();
			node.set(instruction.dest(), std::move(nodeValue));
		}
		if (m_suspensionRequested) {
			m_suspensionRequested = false;
			return RunResult::SUSPENDED;
		}
	}
	return RunResult::FINISHED;
}

std::unique_ptr<ska::bytecode::Executor> ska::bytecode::Interpreter::interpret(std::size_t scriptIndex, GenerationOutput& instructions) {
	auto output = std::make_unique<ska::bytecode::Executor>();
	auto context = ExecutionContext {*output, scriptIndex, instructions };
	interpret(context);
	return output;
}

ska::bytecode::ScriptExecutionOutput ska::bytecode::Interpreter::load(Executor& executor, std::size_t scriptIndex, GenerationOutput& scripts) {
	auto context = ExecutionContext { executor, scriptIndex, scripts };
	interpret(context);
	return context.generateExportedVariables(scriptIndex);
}

ska::NodeValue ska::bytecode::Interpreter::exported(Executor& executor, std::size_t scriptIndex, GenerationOutput& scripts, const std::string& name) {
	const auto exports = load(executor, scriptIndex, scripts);
	const auto& values = exports.nodeval<NodeValueArray>();
	const auto& symbols = scripts.getExportedSymbols(scriptIndex);
	auto index = std::size_t{ 0 };
	for (const auto& symbol : symbols) {
		const auto* exportedSymbol = symbol.value().symbol;
		if (exportedSymbol != nullptr && exportedSymbol->name() == name) {
			if (values == nullptr || index >= values->size()) {
				break;
			}
			const auto& value = (*values)[index];
			return value.resolved();
		}
		index++;
	}
	throw std::runtime_error("no value named \"" + name + "\" is exported by the script \"" + scripts.at(scriptIndex).name() + "\"");
}

ska::bytecode::Coroutine ska::bytecode::Interpreter::start(Executor& executor, GenerationOutput& scripts, const NodeValue& function, std::vector<NodeValue> arguments) {
	if (!function.is<ScriptVariableRef>()) {
		throw std::runtime_error("only a script function can be started as a coroutine");
	}
	auto coroutine = Coroutine { executor, scripts };
	// The arguments are popped by the called function from the last one to the first one
	for (auto it = arguments.rbegin(); it != arguments.rend(); ++it) {
		coroutine.m_thread.stack.push_back(std::move(*it));
	}
	auto call = CallFrame{};
	call.function = function;
	call.returnAddress = HostReturnAddress;
	coroutine.m_thread.frames.push_back(std::move(call));

	// The first executed instruction is the first one of the function
	const auto& address = function.nodeval<ScriptVariableRef>();
	coroutine.m_resumeAddress = ScriptVariableRef{ address.variable - 1, address.script };
	runCoroutine(coroutine, {}, false);
	return coroutine;
}

void ska::bytecode::Interpreter::resume(Coroutine& coroutine, NodeValue value) {
	if (!coroutine.suspended()) {
		throw std::runtime_error("only a suspended coroutine can be resumed");
	}
	runCoroutine(coroutine, std::move(value), true);
}

void ska::bytecode::Interpreter::suspend() {
	if (m_runningCoroutine == nullptr) {
		throw std::runtime_error("cannot suspend : no coroutine is running (a script can only be suspended when called by the host as a coroutine)");
	}
	if (m_nestedTopLevels > 0) {
		throw std::runtime_error("cannot suspend while running the top level code of a script (import)");
	}
	m_suspensionRequested = true;
}

void ska::bytecode::Interpreter::runCoroutine(Coroutine& coroutine, NodeValue resumeValue, bool resuming) {
	auto& executor = *coroutine.m_executor;
	if (executor.coroutineRunning() || m_runningCoroutine != nullptr) {
		throw std::runtime_error("cannot run a coroutine from another running coroutine");
	}

	// The coroutine call frames and value stack are the executor ones while it runs
	struct RunningCoroutine {
		Interpreter& interpreter;
		Executor& executor;
		Coroutine& coroutine;
		RunningCoroutine(Interpreter& interpreter, Executor& executor, Coroutine& coroutine) : interpreter(interpreter), executor(executor), coroutine(coroutine) {
			executor.swapThread(coroutine.m_thread);
			executor.setCoroutineRunning(true);
			interpreter.m_runningCoroutine = &coroutine;
			interpreter.m_nestedTopLevels = 0;
			interpreter.m_suspensionRequested = false;
		}
		~RunningCoroutine() {
			interpreter.m_runningCoroutine = nullptr;
			interpreter.m_suspensionRequested = false;
			executor.setCoroutineRunning(false);
			executor.swapThread(coroutine.m_thread);
		}
	};

	try {
		auto running = RunningCoroutine { *this, executor, coroutine };
		if (resuming) {
			// Value returned by the native function that suspended the coroutine
			executor.push(std::move(resumeValue));
		}

		auto context = ExecutionContext { executor, coroutine.m_resumeAddress.script, *coroutine.m_instructions };
		context.moveTo(coroutine.m_resumeAddress);
		if (run(context, true) == RunResult::SUSPENDED) {
			coroutine.m_resumeAddress = context.currentAddress();
			coroutine.m_state = Coroutine::State::SUSPENDED;
			return;
		}

		auto result = NodeValue{};
		if (!executor.stackEmpty()) {
			executor.pop(result);
		}
		coroutine.m_result = std::move(result);
		coroutine.m_state = Coroutine::State::FINISHED;
	} catch (...) {
		coroutine.m_state = Coroutine::State::FAILED;
		coroutine.m_thread = ExecutionThread{};
		throw;
	}
	coroutine.m_thread = ExecutionThread{};
}
