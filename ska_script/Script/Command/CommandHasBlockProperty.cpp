#include "CommandHasBlockProperty.h"
#include "Data/MemoryScript.h"
#include "Utils/StringUtils.h"
#include "../ScriptUtils.h"
#include "Utils/FormalCalculation/FormalCalculator.h"
#include "World/TileWorld.h"

ska::CommandHasBlockProperty::CommandHasBlockProperty(TileWorld& w, EntityManager& entityManager) :
	AbstractFunctionCommand(entityManager),
	m_world(w) {
}

int ska::CommandHasBlockProperty::argumentsNumber() {
	return 4;
}

std::string ska::CommandHasBlockProperty::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	const auto& layer = StringUtils::fromString<std::size_t>(args[0]);
	const auto& xBlock = StringUtils::strToInt(args[1]);
	const auto& yBlock = StringUtils::strToInt(args[2]);
	const auto& property = StringUtils::fromString<unsigned int>(args[3]);

	const auto& block = m_world.getCollisionProfile().getBlock(layer, xBlock, yBlock);
	if (block != nullptr) {
		const auto properties = block->properties.bitMask;
		const auto hasBlockProperty = (properties & property == property);
		return StringUtils::toString(hasBlockProperty);
	}
	return "";
}
