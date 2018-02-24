#pragma once
namespace ska {
	using ScriptTriggerType = char;
	namespace EnumScriptTriggerType
	{
		static const ScriptTriggerType AUTO = 0;
		static const ScriptTriggerType ACTION = 1;
		static const ScriptTriggerType MOVE_IN = 2;
		static const ScriptTriggerType MOVE_OUT = 3;
		static const ScriptTriggerType TOUCH = 4;
		static const ScriptTriggerType NONE = 5;
	}
}
