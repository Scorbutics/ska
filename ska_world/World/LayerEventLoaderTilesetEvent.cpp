#include "LayerEventLoaderTilesetEvent.h"
#include "Layer.h"

ska::LayerEventLoaderTilesetEvent::LayerEventLoaderTilesetEvent(const Layer& layer, const TilesetEvent& tilesetEvent) :
	m_layer(layer),
	m_tilesetEvent(tilesetEvent) {
}

ska::Vector2<ska::ScriptPack> ska::LayerEventLoaderTilesetEvent::loadPositioned(unsigned int width, unsigned int height) const {
	constexpr ScriptTriggerType allValidScriptTypes[] = { ScriptTriggerType::TOUCH, ScriptTriggerType::ACTION, ScriptTriggerType::MOVE_IN, ScriptTriggerType::MOVE_OUT };

	auto events = ska::Vector2<ScriptPack>{};
	events.resize(width, height);

	for (auto x = 0u; x < width; x++ ) {
		for (auto y = 0u; y < height; y++) {
			const auto& b = m_layer.getBlock(x, y);
			if (b != nullptr) {
				for (const auto& scriptType : allValidScriptTypes) {
					auto scripts = m_tilesetEvent.getScript(scriptType, b->id);
					for (const auto& s : scripts) {
						events[x][y].push_back(s);
					}
				}
			}
		}
	}
	return events;
}

ska::ScriptPack ska::LayerEventLoaderTilesetEvent::loadGlobal() const {
	auto autosScript = m_tilesetEvent.getScript(ScriptTriggerType::AUTO);
	auto result = ScriptPack{};
	if (!autosScript.empty()) {
		result.reserve(autosScript.size());
		for (const auto& s : autosScript) {
			result.push_back(s);
		}
	}
	return result;
}

const std::string& ska::LayerEventLoaderTilesetEvent::getName() const {
    return "tilesetEvent";
}
