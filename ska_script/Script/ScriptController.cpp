#include <fstream>
#include "ScriptController.h"
#include "Exceptions/InvalidPathException.h"

std::string ska::ScriptController::nextLine() {
	std::string line = file[currentLine];
	currentLine++;
	return line;
}

ska::ScriptController::ScriptController(const std::string& scriptFileName) {
    std::ifstream scriptFile(scriptFileName);
    if (scriptFile.fail()) {
        throw InvalidPathException(("Impossible d'ouvrir le fichier script " + scriptFileName).c_str());
    }

    for (std::string line; getline(scriptFile, line);) {
        file.push_back(line);
    }
}

void ska::ScriptController::rewind() {
    currentLine = 0;
}

bool ska::ScriptController::eof() const {
	return file.size() <= currentLine;
}
