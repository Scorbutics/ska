#pragma once
#include <string>
#include <utility>
#include <vector>

namespace ska {
	class MemoryScript {
	public:
		MemoryScript() = default;
        virtual ~MemoryScript() = default;

		virtual std::string getSaveName() const = 0;
		virtual int getGameVariable(const std::string& var) const = 0;

		virtual void setGameVariable(const std::string& var, int value) = 0;
		// Every game variable, sorted by name (for the tools : the development console)
		virtual std::vector<std::pair<std::string, int>> getGameVariables() const { return {}; }

	};
}
