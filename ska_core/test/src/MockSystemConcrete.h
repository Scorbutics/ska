#pragma once
#include "ExposeImmutable.h"
#include "Core/ECS/System.h"

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
	using SystemParent = ska::System <ska::RequiredComponent<RComponentType...>, ska::PossibleComponent<PComponentType...>>;
public:
	using SystemParent::System;

private:
	void refresh(unsigned int) {
		refresh_ = true;
		entitiesRefreshed_ = SystemParent::getEntities();
	}

};