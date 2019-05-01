#pragma once
#include <string>
#include <optional>
#include "TilesetEventLoader.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class TilesetEventLoaderText : 
		public TilesetEventLoader {
	public:
		TilesetEventLoaderText(std::string tilesetName);

		std::vector<ScriptSleepComponent> load() const override;
		static void addScriptIfExists(std::vector<ska::ScriptSleepComponent>& scripts, std::optional<ska::ScriptSleepComponent> s);
		static std::optional<ska::ScriptSleepComponent> fillScript(const std::string& chipsetFolder, const std::string& id, const ScriptTriggerType& type);
	private:
		std::string m_tilesetName;
	};
}
