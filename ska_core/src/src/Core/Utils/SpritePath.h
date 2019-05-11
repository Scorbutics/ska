#pragma once

#include <string>
#include <vector>
#include "Base/Patterns/Singleton.h"

namespace ska {
	class SpritePath : public Singleton<SpritePath> {
		friend class Singleton<SpritePath>;

	private:
		std::vector<std::string> m_paths;
		SpritePath();
		~SpritePath();

	public:
		std::string getPath(unsigned int spritebankType, int id);


	};
}
