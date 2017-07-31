#include "CollisionComponent.h"

ska::CollisionComponent::CollisionComponent(): 
	overlap(ska::Rectangle{ 0, 0, 0, 0 }),
	penetration(0) {
	xaxis = false;
	yaxis = false;
	origin = 0;
	target = 0;
}
