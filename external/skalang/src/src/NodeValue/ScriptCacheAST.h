#pragma once

#include "Base/Containers/order_indexed_string_map.h"

namespace ska {
	class ScriptHandleAST;
	struct ScriptCacheAST :
		public order_indexed_string_map<ScriptHandleAST> {
	};
}
