#pragma once
#include <string>

namespace ska {
    class StateHolder;

	class Module {
		friend class GameConfiguration;

	public:
		Module(Module&&) = default;
		virtual void eventUpdate(unsigned int ellapsedTime) {};
		virtual void graphicUpdate(unsigned int ellapsedTime, StateHolder& sh) {};
		virtual ~Module();

	protected:
		explicit Module(const std::string& name);

	private:
		const std::string m_name;
	};
}
