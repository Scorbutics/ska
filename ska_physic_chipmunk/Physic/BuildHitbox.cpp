#include "BuildHitbox.h"
#include "Constraint.h"
#include "Shape.h"
#include "Body.h"
#include "Space.h"

namespace ska {
	namespace cp {
		ska::cp::HitboxComponent BuildCircleHitbox(ska::cp::Space& space, const ska::Point<float>& point, float friction, float rotateFriction, EntityId entityId) {
			/*const auto ballBodyIndex = space.addBody(ska::cp::Body::fromRadius(1., 16.));
			auto& ballBody = space.getBodies()[ballBodyIndex];
			ballBody.setPosition(point);
			ballBody.setEntity(entityId);

			const auto ballShapeIndex = space.addShape(ska::cp::Shape::fromCircle(ballBody.body(), 16.));

			ska::cp::HitboxComponent bc{};
			bc.bodyIndex = ballBodyIndex;
			bc.shapeIndex = ballShapeIndex;

			{
				auto pjIndex = space.addConstraint(ska::cp::Constraint::buildPivotJoint(*space.getStaticBody(), *ballBody.body()));
				auto& pj = space.getConstaints()[pjIndex];
				pj.setMaxForce(friction);
				pj.setMaxBias(0);
			}
    
			{
				auto gjIndex = space.addConstraint(ska::cp::Constraint::buildGearJoint(*space.getStaticBody(), *ballBody.body(), 0., 1.));
				auto& gj = space.getConstaints()[gjIndex];
				gj.setMaxForce(rotateFriction);
				gj.setMaxBias(0);
			}

			return bc;*/
			return {};
		}

		ska::cp::HitboxComponent BuildRectangleHitbox(ska::cp::Space& space, const ska::Rectangle& box, float friction, float rotateFriction, EntityId entityId) {
			const auto ballControlBodyIndex = space.addBody(ska::cp::Body::fromKinematic());

			const auto ballBodyIndex = space.addBody(ska::cp::Body::fromMoment(1., INFINITY));
			const auto ballShapeIndex = space.addShape(ska::cp::Shape::fromBox(space.getBodies()[ballBodyIndex].body(), {0, 0, box.w, box.h}));

			{
				auto& ballBody = space.getBodies()[ballBodyIndex];
				ballBody.setPosition({ static_cast<float>(box.x), static_cast<float>(box.y) });
				ballBody.setEntity(entityId);
			}

			
			auto& ballControlBody = space.getBodies()[ballControlBodyIndex];		
			ballControlBody.setPosition(space.getBodies()[ballBodyIndex].getPosition());
			ballControlBody.setEntity(entityId);

			{
				auto pjIndex = space.addConstraint(ska::cp::Constraint::buildPivotJoint(*ballControlBody.body(), *space.getBodies()[ballBodyIndex].body()));
				auto& pj = space.getConstaints()[pjIndex];
				pj.setMaxForce(friction);
				pj.setMaxBias(0);
			}
    
			{
				auto gjIndex = space.addConstraint(ska::cp::Constraint::buildGearJoint(*ballControlBody.body(), *space.getBodies()[ballBodyIndex].body(), 0., 1.));
				auto& gj = space.getConstaints()[gjIndex];
				gj.setMaxForce(rotateFriction);
				gj.setErrorBias(0);
				gj.setMaxBias(0);
			}
			
			ska::cp::HitboxComponent bc{};
			bc.bodyIndex = ballControlBodyIndex;
			bc.shapeIndex = ballShapeIndex;

			return bc;
		}
	}
}
