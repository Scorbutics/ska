#pragma once
#include "ExposeImmutable.h"
#include "ECS/System.h"

struct ImmutableSystemConcreteTracer {
	bool refresh_ = false;
	std::unordered_set<ska::EntityId> entitiesRefreshed_;
};

template <class RComponentType, class PComponentType>
class MockSystemConcrete;

template <class ... RComponentType, class ... PComponentType>
class MockSystemConcrete<ska::RequiredComponent<RComponentType...>, ska::PossibleComponent<PComponentType...>> :
	public ska::System <ska::RequiredComponent<RComponentType...>, ska::PossibleComponent<PComponentType...>>,
	public ExposeImmutable<ImmutableSystemConcreteTracer> {

public:
	using System::System;

private:
	void refresh(unsigned int) {
		refresh_ = true;
		entitiesRefreshed_ = getEntities();
	}

};