#pragma once
#include <unordered_set>
#include <functional>
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
		// The priority of a sprite (the drawing order) : its ground line, the bottom of its hitbox (its feet : the same for
		// any size of sprite ; without a hitbox, the bottom of the sprite : an effect, a thrown ball). It is sorted with the
		// standing tiles of the maps by their base line (ska::LayerDepthRenderable) : drawn behind an object whose base is
		// in front of its feet, whatever the height of the object.
		// A sprite at "height" or higher (in the air, ex : flying over the tree tops) is drawn from "priority" (+ its ground
		// line) : over the standing tiles. Lower (a hop, a landing), it is sorted on the ground.
		// Without an air layer (height 0), the height is added to the priority (camera height * z).
		void setAirLayer(long height, int priority);
		// The priority of the silhouettes (GraphicComponent::silhouette) : over the world, under its interface
		void setSilhouettePriority(int priority) { m_silhouettePriority = priority; }
		// The entity is drawn by this system (it has the components it needs : a debug view)
		bool draws(EntityId entity) const { return getEntities().count(entity) > 0; }
		int spritePriority(long ground, long z, int cameraHeight) const;
        virtual void update(unsigned int ellapsedTime) override;
        virtual ~GraphicSystem();
    protected:
        virtual void refresh(unsigned int ellapsedTime) override;
    private:
		ska::GameEventDispatcher& m_ged;
        int m_topLayerPriority;
		long m_airHeight = 0;
		int m_airPriority = 0;
		int m_silhouettePriority = 0;
    };
}
