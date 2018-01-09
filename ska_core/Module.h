#pragma once
#include <string>

namespace ska {
	class Module {
		friend class GameConfiguration;

	public:
		Module(Module&&) = default;
		virtual ~Module();

	protected:
		explicit Module(const std::string& name);

	private:
		const std::string m_name;
	};
}
