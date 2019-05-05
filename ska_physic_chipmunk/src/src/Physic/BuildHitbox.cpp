#include <chipmunk.h>
#include "BuildHitbox.h"
#include "Constraint.h"
#include "Shape.h"
#include "Body.h"
#include "Space.h"

namespace ska {
	namespace cp {
		ska::cp::HitboxComponent BuildCircleHitbox(Space& space, const ska::Point<float>& point, float friction, float rotateFriction, EntityId entityId) {
			/*
			const auto ballBodyIndex = space.addBody(ska::cp::Body::fromRadius(1., 16.));
			const auto ballShapeIndex = space.addShape(ska::cp::Shape::fromCircle(ballBody.body(), 16.));
			*/
			return {};
		}

		void AddTopDownConstraints(Space& space, HitboxComponent& hc, const float friction, const float rotateFriction) {
			auto& bodyToBeLinked = space.getBody(hc.controlBodyIndex);
			auto& bodyCreated = space.getBody(hc.bodyIndex);

			{
				auto pjIndex = space.addConstraint(&bodyToBeLinked, bodyCreated, ska::cp::Constraint::buildPivotJoint(bodyToBeLinked.body(), bodyCreated.body()));
				auto& pj = space.getConstaint(pjIndex);
				pj.setMaxForce(friction);
				pj.setMaxBias(0);
			}

			{
				auto gjIndex = space.addConstraint(&bodyToBeLinked, bodyCreated, ska::cp::Constraint::buildGearJoint(bodyToBeLinked.body(), bodyCreated.body(), 0., 1.));
				auto& gj = space.getConstaint(gjIndex);
				gj.setMaxForce(rotateFriction);
				gj.setErrorBias(0);
				gj.setMaxBias(0);
			}
		}

		ska::cp::HitboxComponent BuildRectangleHitbox(Space& space, const ska::Point<float>& pos, const ska::Rectangle& box, double mass, EntityId entityId) {
			const auto ballBodyIndex = space.addBody(Body::fromMoment(mass / 65.F, INFINITY));
			const auto ballShapeIndex = space.addShape(&space.getBody(ballBodyIndex), Shape::fromBox(space.getBody(ballBodyIndex).body(), {0, 0, box.w, box.h}));

			{
				auto& ballBody = space.getBody(ballBodyIndex);
				ballBody.setPosition(pos);
				ballBody.setEntity(entityId);
			}
			
			ska::cp::HitboxComponent bc{};
			bc.bodyIndex = ballBodyIndex;
			bc.controlBodyIndex = ballBodyIndex;
			bc.shapeIndex = ballShapeIndex;

			return bc;
		}

		ska::cp::HitboxComponent BuildControlledRectangleHitbox(Space& space, const ska::Point<float>& pos, const ska::Rectangle& box, double mass, EntityId entityId) {
			auto hc = BuildRectangleHitbox(space, pos, box, mass, entityId);

			const auto controlBodyIndex = space.addBody(Body::fromKinematic());
			space.getBody(controlBodyIndex).setEntity(entityId);
			hc.controlBodyIndex = controlBodyIndex;

			return hc;
		}
	}
}
