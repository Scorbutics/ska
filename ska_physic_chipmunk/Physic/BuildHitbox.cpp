#include "BuildHitbox.h"
#include "Constraint.h"
#include "Shape.h"
#include "Body.h"
#include "Space.h"

namespace ska {
	namespace cp {
		ska::cp::HitboxComponent BuildCircleHitbox(ska::cp::Space& space, const ska::Point<float>& point, float friction, float rotateFriction, EntityId entityId) {
			auto& ballBody = space.addBody(ska::cp::Body::fromRadius(1., 16.));
			const auto ballBodyIndex = space.getBodies().size() - 1;
			ballBody.setPosition(point);
			ballBody.setEntity(entityId);

			space.addShape(ska::cp::Shape::fromCircle(ballBody.body(), 16.));
			const auto ballShapeIndex = space.getShapes().size() - 1;
			//ballShape.setFriction(0.7F);

			ska::cp::HitboxComponent bc{};
			bc.bodyIndex = ballBodyIndex;
			bc.shapeIndex = ballShapeIndex;

			auto& pj = space.addConstraint(ska::cp::Constraint::buildPivotJoint(*space.getStaticBody(), *ballBody.body()));
			pj.setMaxForce(friction);   
			pj.setMaxBias(0); 
    
			auto& gj = space.addConstraint(ska::cp::Constraint::buildGearJoint(*space.getStaticBody(), *ballBody.body(), 0., 1.));
			gj.setMaxForce(rotateFriction);
			gj.setMaxBias(0);

			return bc;
		}

		ska::cp::HitboxComponent BuildRectangleHitbox(ska::cp::Space& space, const ska::Rectangle& box, float friction, float rotateFriction, EntityId entityId) {
			auto& ballBody = space.addBody(ska::cp::Body::fromMoment(1., INFINITY));
			const auto ballBodyIndex = space.getBodies().size() - 1;
			ballBody.setPosition({static_cast<float>(box.x), static_cast<float>(box.y)});
			ballBody.setEntity(entityId);

			auto& ballShape = space.addShape(ska::cp::Shape::fromBox(ballBody.body(), {0, 0, box.w, box.h}));
			const auto ballShapeIndex = space.getShapes().size() - 1;
			//ballShape.setFriction(0.7F);

			ska::cp::HitboxComponent bc{};
			bc.bodyIndex = ballBodyIndex;
			bc.shapeIndex = ballShapeIndex;

			auto& pj = space.addConstraint(ska::cp::Constraint::buildPivotJoint(*space.getStaticBody(), *ballBody.body()));
			pj.setMaxForce(friction);   
			pj.setMaxBias(0); 
    
			auto& gj = space.addConstraint(ska::cp::Constraint::buildGearJoint(*space.getStaticBody(), *ballBody.body(), 0., 1.));
			gj.setMaxForce(rotateFriction);
			gj.setMaxBias(0);

			return bc;
		}
	}
}
