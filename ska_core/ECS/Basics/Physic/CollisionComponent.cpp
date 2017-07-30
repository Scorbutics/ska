#include "CollisionComponent.h"

ska::CollisionComponent::CollisionComponent(): 
	penetration(0),
	overlap(ska::Rectangle{ 0, 0, 0, 0 }) {
	xaxis = false;
	yaxis = false;
	origin = 0;
	target = 0;
}
