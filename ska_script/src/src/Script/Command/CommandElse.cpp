#include <string>
#include "CommandElse.h"
#include "CommandIf.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Exceptions/ScriptSyntaxError.h"
#include "../ScriptComponent.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandElse::CommandElse(EntityManager& entityManager) : 
	ControlStatement(entityManager) {
}

const std::string& ska::CommandElse::getCmdName() {
	return getCommandElse();
}

std::string ska::CommandElse::analyzeLine(ScriptComponent& script, std::stringstream&, const std::vector<std::string>&) {
	int ifEnd = 1;
	std::string lineBuf;

	//TODO le control flow devrait se faire depuis un controlleur intégré directement dans le script component (à allouer sur le tas pour éviter la lourdeur du composant)
	while (ifEnd > 0 && !script.controller->eof()) {
		lineBuf = StringUtils::trim(script.controller->nextLine());
		if (lineBuf.find(getCommandIf()) == 0)
			ifEnd++;
		else if (lineBuf == getCommandEndIf())
			ifEnd--;
	}

	if (script.controller->eof()) {
		throw ScriptSyntaxError("Un endif est manquant");
	}

	return "";
}
