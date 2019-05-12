#include <unordered_set>
#include <doctest.h>
#include "LoggerConfig.h"
#include "MockSystemConcrete.h"

TEST_CASE("[System]") {
	auto ged = ska::GameEventDispatcher{};
	auto&& em = ska::EntityManager::Make<int, std::string, double> ( ged );

	class ComponentAltererObservable :
		public ska::Observable<const ska::EntityEventType> {
	};
	auto componentAltererObservable = ComponentAltererObservable{};

	SUBCASE("int std::string required, double possible") {
		auto system = MockSystemConcrete<ska::RequiredComponent<int, std::string>, ska::PossibleComponent<double>>{ em };
		componentAltererObservable.addObserver(system);

		//We first check that our system is empty
		system.update(0);
		CHECK(system.read.entitiesRefreshed_.empty());

		//Describes an entity with 2 components (int and std::string)
		//A group of components id is called a system mask
		ska::EntityComponentsMask mask;
		mask[em.getMask<int>()] = true;
		mask[em.getMask<std::string>()] = true;
		
		//As the system masks requirements are matching (the one provided in parameter and the system internal one), the system will now register the entity with id "0"
		auto event = ska::EntityEventType{ ska::EntityEventTypeEnum::COMPONENT_ALTER, mask, 0 };
		componentAltererObservable.notifyObservers(event);
		
		system.update(0);
		CHECK(system.read.entitiesRefreshed_.size() == 1);

		//Now, we alter the previously created system mask by "removing" the first required component (int)
		mask[em.getMask<int>()] = false;

		//As the system masks requirements are no longer matching (the one provided in parameter and the system internal one), the system will now unregister the entity with id "0"
		componentAltererObservable.notifyObservers(event);
		
		system.update(0);
		CHECK(system.read.entitiesRefreshed_.empty());
	}

	SUBCASE("scheduleDeferredRemove") {
		auto system = MockSystemConcrete<ska::RequiredComponent<int>, ska::PossibleComponent<>>{ em };
		componentAltererObservable.addObserver(system);

		auto entity = em.createEntity();
		em.addComponent(entity, 130);
		em.refresh();

		system.update(0);
		CHECK(system.read.entitiesRefreshed_.size() == 1);

		system.scheduleDeferredRemove(entity);

		//First, update the system for removing
		system.update(0);

		//Then update the EntityManager
		em.refresh();

		//Then we have to update the system again
		system.update(0);

		CHECK(system.read.entitiesRefreshed_.empty());
	}
}