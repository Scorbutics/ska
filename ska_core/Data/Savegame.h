#pragma once
#include <string>

namespace ska {
	class Savegame
	{
	public:
		Savegame() {}

		virtual std::string getSaveName() const = 0;
		virtual int getGameVariable(const std::string& var) const = 0;
		virtual bool getGameSwitch(const std::string& swi) const = 0;

		virtual void setGameVariable(const std::string& var, const int value) = 0;
		virtual void setGameSwitch(const std::string& swi, const bool value) = 0;

		virtual ~Savegame() {}
	};
}
