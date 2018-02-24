#include "Physic/Vect.h"
#include "Physic/Shape.h"
#include "Physic/Body.h"
#include "Physic/Space.h"

#include ""

int main() {
	using ska::cp::Vect;
	using ska::cp::Body;
	using ska::cp::Shape;
	using ska::cp::Space;

	const auto gravity = Vect { 0, -100 };

	Space space;
	space.setGravity(gravity);

	auto& ground = space.addShape(Shape::fromSegment(space.getStaticBody(), Vect{ -20, 5 }, Vect{ 20, -5 }, 0));
	ground.setFriction(1);

	const auto radius = 5.F;
	const auto mass = 1.F;

	auto& ballBody = space.addBody(Body::fromRadius(mass, radius));
	ballBody.setPosition(Vect{ 0, 15 });

	auto& ballShape = space.addShape(Shape::fromCircle(ballBody.body(), radius, Vect{}));
	ballShape.setFriction(0.7);

	const auto timeStep = 1.0 / 60.0;
	for (auto time = 0.; time < 2.; time += timeStep) {
		const auto pos = ballBody.getPosition();
		const auto vel = ballBody.getVelocity();
		printf(
			"Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
			time, pos.x, pos.y, vel.x, vel.y
		);

		space.step(timeStep);
	}

	return 0;
}