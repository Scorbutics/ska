#pragma once
#include <string>
#include <vector>

namespace ska {
    class ScriptComponent;

	class ScriptController {
	public:
		ScriptController() = default;
		ScriptController(const std::string& scriptFileName);
		~ScriptController() = default;

		std::string nextLine();
		void rewind();
        bool eof() const;

        inline unsigned int getCurrentLine() const {
            return currentLine;
        }

    private:
        unsigned int currentLine;
		std::vector<std::string> file;
	};
}
