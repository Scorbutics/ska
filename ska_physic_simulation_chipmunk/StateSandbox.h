#pragma once
#include <memory>
#include "Data/Events/ExtensibleGameEventDispatcher.h"
#include "ECS/EntityManager.h"
#include "Core/State/StateBase.h"
#include "Utils/SubObserver.h"
#include "Graphic/Polygon.h"
#include "Physic/Space.h"
#include "Graphic/PositionnedGraphicDrawable.h"
#include "World/Tileset.h"
#include "Graphic/System/CameraSystem.h"
#include "Physic/SpaceCollisionEventSender.h"

class LayerHolder : public ska::DrawableFixedPriority {
public:
	void render(const ska::Renderer& renderer) const override{
		const auto width = layerRenderableBlocks.lineSize();
		const auto height = width == 0 ? 0 : layerRenderableBlocks.size() / width;
		for (auto x = 0; x < width; x++) {
			for (auto y = 0; y < height; y++) {
				const auto& b = layerRenderableBlocks[x][y];
				
				chipset->getTexture().render(renderer, x * 48, y * 48, &b, 0, nullptr);
				
			}
		}
	}

	ska::Vector2<ska::Rectangle> layerRenderableBlocks;
	std::unique_ptr<ska::Tileset> chipset;
};

class StateSandbox :
	public ska::StateBase,
	public ska::SubObserver<ska::GameEvent>,
	public ska::SubObserver<ska::InputMouseEvent> {
public:
	StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>&);

	virtual ~StateSandbox() = default;

	void onGraphicUpdate(unsigned int, ska::DrawableContainer&) override;
	void onEventUpdate(unsigned int) override;

private:
	void createBall(const ska::Point<float>& point);
	bool onMouseEvent(ska::InputMouseEvent& ime);
	bool onGameEvent(ska::GameEvent& ge);

	LayerHolder m_layerHolder;
	ska::CameraSystem* m_cameraSystem{};
	
	ska::ExtensibleGameEventDispatcher<>& m_eventDispatcher;
	ska::EntityManager& m_entityManager;
	std::vector<ska::Polygon<int>> m_layerContours{};
	
	ska::Texture m_ballTexture;
	
	ska::cp::Space m_space;
	std::vector<ska::PositionnedGraphicDrawable> m_ballGraphics{};
	std::vector<ska::EntityId> m_balls{};

	ska::cp::SpaceCollisionEventSender m_spaceCollisionEventSender;

};

