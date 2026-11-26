#pragma once

#include "Generator/BytecodeCommand.h"
#include "BytecodeInterpreter.h"
#include "InterpreterCommandUnit.h"
#include "BytecodeExecutionContext.h"

namespace ska {
	namespace bytecode {
		template <Command cmd>
		class InterpreterCommand;
	}
}

#define SKALANG_BYTECODE_INTERPRETER_COMMAND_DEFINE(CommandType)\
	template<>\
	class InterpreterCommand<CommandType> :\
		public InterpreterCommandBase {\
	public:\
		using InterpreterCommandBase::InterpreterCommandBase;\
		ScriptExecutionOutput interpret(ExecutionContext& context) override final {\
			return interpret(context, context.currentInstruction().left(), context.currentInstruction().right());\
		}\
		ScriptExecutionOutput interpret(ExecutionContext& context, const Operand& left, const Operand& right);\
	};

#define SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CommandType)\
	ska::bytecode::ScriptExecutionOutput ska::bytecode::InterpreterCommand<ska::bytecode::Command::CommandType>::interpret
