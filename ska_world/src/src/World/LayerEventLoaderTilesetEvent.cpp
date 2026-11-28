#include <unordered_map>
#include "LayerEventLoaderTilesetEvent.h"
#include "Layer.h"
#include "CollisionProfile.h"
#include "Core/Exceptions/IllegalArgumentException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Base/Values/Strings/StringUtils.h"

ska::LayerEventLoaderTilesetEvent::LayerEventLoaderTilesetEvent(const CollisionProfile& collisionProfile, const std::size_t layer, const TilesetEvent& tilesetEvent) :
	m_collisionProfile(collisionProfile),
	m_layer(layer),
	m_tilesetEvent(tilesetEvent),
	m_name("tilesetEvent") {

	if(m_collisionProfile.layers() <= m_layer) {
		ExceptionTrigger<IllegalArgumentException>("The chosen layer doesn't exist : its index is too high (index " + 
				StringUtils::toString(m_layer) + " of " + StringUtils::toString(m_collisionProfile.layers()) + " layers available in total)" );
	}
}

ska::Vector2<ska::ScriptPack> ska::LayerEventLoaderTilesetEvent::loadPositioned(unsigned int width, unsigned int height) const {
	constexpr ScriptTriggerType allValidScriptTypes[] = { ScriptTriggerType::TOUCH, ScriptTriggerType::ACTION, ScriptTriggerType::MOVE_IN, ScriptTriggerType::MOVE_OUT };

	auto events = ska::Vector2<ScriptPack>{};
	events.resize(width, height);

	// By tile : looked up once, the blocks of a map share a few tiles
	auto byTile = std::unordered_map<Point<int>, ScriptPack>{};
	for (auto x = 0u; x < width; x++ ) {
		for (auto y = 0u; y < height; y++) {
			const auto& b = m_collisionProfile.getBlock(m_layer, x, y);
			if (b == nullptr) {
				continue;
			}
			auto found = byTile.find(b->id);
			if (found == byTile.end()) {
				found = byTile.emplace(b->id, ScriptPack{}).first;
				for (const auto& scriptType : allValidScriptTypes) {
					for (const auto& s : m_tilesetEvent.getScript(scriptType, b->id)) {
						found->second.push_back(s);
					}
				}
			}
			if (!found->second.empty()) {
				events[x][y] = found->second;
			}
		}
	}
	return events;
}

ska::ScriptGlobalPack ska::LayerEventLoaderTilesetEvent::loadGlobal() const {
	auto autosScript = m_tilesetEvent.getScript(ScriptTriggerType::AUTO);
	auto result = ScriptGlobalPack{};
	if (!autosScript.empty()) {
		result.reserve(autosScript.size());
		for (const auto& s : autosScript) {
			result.push_back({ ska::BlockEvent{}, s });
		}
	}
	return result;
}

const std::string& ska::LayerEventLoaderTilesetEvent::getName() const {
    return m_name;
}
