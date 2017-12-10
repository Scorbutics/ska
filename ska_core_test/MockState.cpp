#include "MockState.h"

MockState::MockState(ska::StateHolder& sh) : 
	ska::State(sh) {
}

void MockState::graphicUpdate(unsigned ellapsedTime, ska::DrawableContainer& drawables){
	
}

void MockState::eventUpdate(unsigned){
	
}

void MockState::load(std::unique_ptr<State>* lastState){
	
}

bool MockState::unload(){
	return false;
}


