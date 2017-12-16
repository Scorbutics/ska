#pragma once
#include <string>

namespace ska {
	class Module {
		friend class GameConfiguration;

	public:
		virtual ~Module();
		Module(Module&&) = default;

	protected:
		explicit Module(const std::string& name);
	
	private:
		const std::string m_name;
	};
}
