#include <string>
#include "CommandElse.h"
#include "CommandIf.h"
#include "Utils/StringUtils.h"
#include "Exceptions/ScriptSyntaxError.h"
#include "../ScriptComponent.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandElse::CommandElse(EntityManager& entityManager) : ControlStatement(entityManager)
{
}


ska::CommandElse::~CommandElse()
{
}

const std::string& ska::CommandElse::getCmdName() {
	return getCommandElse();
}

std::string ska::CommandElse::analyzeLine(ScriptComponent& script, std::stringstream&, std::vector<std::string>&) {
	int ifEnd = 1;
	std::string lineBuf;

	//TODO le control flow devrait se faire depuis un controlleur int�gr� directement dans le script component (� allouer sur le tas pour �viter la lourdeur du composant)
	while (ifEnd > 0 && !script.controller->eof()) {
		lineBuf = script.controller->nextLine();
		StringUtils::ltrim(lineBuf);
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
