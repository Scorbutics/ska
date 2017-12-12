#pragma once
#include <string>
#include <vector>

namespace ska {
	class Module {
		friend class GameConfiguration;

	public:
		virtual ~Module();

	protected:
		explicit Module(const std::string& name);
	
	private:
		std::string m_name;
	};
}
