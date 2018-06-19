#pragma once
#include <string>

namespace ska {
	class MemoryScript {
	public:
		MemoryScript() = default;
        virtual ~MemoryScript() = default;

		virtual std::string getSaveName() const = 0;
		virtual int getGameVariable(const std::string& var) const = 0;
		virtual std::string getComponentVariable(const std::string& var) const = 0;

		virtual void setGameVariable(const std::string& var, int value) = 0;		
		virtual void setComponentVariable(const std::string& var, const std::string& value) = 0;

	};
}
