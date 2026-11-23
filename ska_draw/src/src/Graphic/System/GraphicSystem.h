#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/System.h"
#include "AbstractGraphicSystem.h"
#include <Core/Data/Events/EventDispatcher.h>
#include <Core/Data/Events/GameEventDispatcher.h>
#include "Core/ECS/Basics/Graphic/DialogComponent.h"

namespace ska {
    class GraphicSystem : public AbstractGraphicSystem, 
		public System< RequiredComponent<GraphicComponent, PositionComponent, HitboxComponent>, PossibleComponent<DialogComponent>> {
    public:
		GraphicSystem(EntityManager& entityManager, ska::GameEventDispatcher& ged, CameraSystem& camera);
        GraphicSystem& operator=(const GraphicSystem&) = delete;
        int getTopLayerPriority() const;
		// The priority of a sprite (the drawing order) : its position on the ground (y), and its height (z). A sprite at
		// "height" or higher (in the air, ex : flying) is drawn from "priority" (+ y) : over the layers of the map drawn
		// over the characters (tree tops...). Lower (on the ground, a hop), it is sorted on the ground by its y.
		// Without an air layer (height 0), the height is added to the priority (camera height * z).
		void setAirLayer(long height, int priority);
		int spritePriority(long groundY, long z, int cameraHeight) const;
        virtual void update(unsigned int ellapsedTime) override;
        virtual ~GraphicSystem();
    protected:
        virtual void refresh(unsigned int ellapsedTime) override;
    private:
		ska::GameEventDispatcher& m_ged;
        int m_topLayerPriority;
		long m_airHeight = 0;
		int m_airPriority = 0;
    };
}
