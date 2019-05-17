#include <algorithm>
#include <doctest.h>
#include "LoggerConfig.h"
#include "Core/ECS/EntityManager.h"

struct ComponentTest1 {
	int t;
};

struct ComponentTest2 {
	char* t;
};

SKA_DEFINE_COMPONENT(ComponentTest1);
SKA_DEFINE_COMPONENT(ComponentTest2);

bool EntityManagerTestIsEntityRemoved(ska::ECSEvent& event, const ska::EntityId& entity) {
	if (event.ecsEventType == ska::ECSEventType::ENTITIES_REMOVED) {
		auto& entitiesRemoved = event.entities;
		auto foundEntity = std::find_if(entitiesRemoved.begin(), entitiesRemoved.end(), [&](const auto& t) {
			return t.first == entity;
		});
		return foundEntity != entitiesRemoved.end();
	}
	return false;
}

TEST_CASE("[EntityManager]") {
	auto ged = ska::GameEventDispatcher {};
	auto&& em = ska::EntityManager{ged};
	
	using EntityManagerObserver = ska::Observer<ska::ECSEvent>;
	class EntityManagerObserverTest :
		public EntityManagerObserver {
	public:
		EntityManagerObserverTest() : EntityManagerObserver(std::bind(&EntityManagerObserverTest::onEntityEvent, this, std::placeholders::_1)) {}
	
		std::vector<ska::ECSEvent> events;
	private:
		bool onEntityEvent(const ska::ECSEvent& event) {
			events.push_back(event);
			return true;
		}

	};

	using ComponentObserverParentTest = ska::Observer<const ska::EntityEventType>;

	struct ComponentAlterDataEvent {
		ska::EntityEventTypeEnum event;
		ska::EntityComponentsMask mask;
		ska::EntityId entityId;
	};

	class ComponentObserverTest : 
		public ComponentObserverParentTest {
	public:
		ComponentObserverTest() : 
			ComponentObserverParentTest(std::bind(&ComponentObserverTest::onComponentModified, this, std::placeholders::_1)) {
		}

		bool onComponentModified(const ska::EntityEventType& event) {
			eventType.push_back(ComponentAlterDataEvent { event.value, event.mask, event.id });
			return true;
		}

		std::vector<ComponentAlterDataEvent> eventType;
	};
	
	auto componentObserver = ComponentObserverTest {};
	auto entityObserver = EntityManagerObserverTest{};
	ged.ska::Observable<ska::ECSEvent>::addObserver(entityObserver);
	em.addObserver(componentObserver);

	SUBCASE("AddComponent + GetComponent + refresh") {
		ska::EntityId entity = 0;
		em.addComponent(entity, ComponentTest1{14});
		
		auto component = em.getComponent<ComponentTest1>(entity);

		CHECK(componentObserver.eventType.empty());

		em.refresh();

		CHECK(componentObserver.eventType.size() == 1);
		CHECK(componentObserver.eventType[0].event == ska::EntityEventTypeEnum::COMPONENT_ALTER);
		CHECK(componentObserver.eventType[0].entityId == entity);

		CHECK(component.t == 14);
	}

	SUBCASE("createEntity (differents)") {
		ska::EntityId entity1 = em.createEntity();
		ska::EntityId entity2 = em.createEntity();
		
		em.refresh();

		CHECK(componentObserver.eventType.size() == 2);
		CHECK(componentObserver.eventType[0].event == ska::EntityEventTypeEnum::COMPONENT_ALTER);
		CHECK(componentObserver.eventType[0].entityId == entity2);
		CHECK(componentObserver.eventType[1].event == ska::EntityEventTypeEnum::COMPONENT_ALTER);
		CHECK(componentObserver.eventType[1].entityId == entity1);
		
		CHECK(entity1 != entity2);
	}

	SUBCASE("getMask (differents)") {
		auto maskNotInitialized = em.getMask<ComponentTest1>();
		auto mask2 = em.getMask<ComponentTest2>();
		CHECK(mask2 != maskNotInitialized);
	}

	SUBCASE("hasComponent") {
		ska::EntityId entity = 0;
		CHECK(!em.hasComponent<ComponentTest1>(entity));
		em.addComponent<ComponentTest1>(entity, ComponentTest1());
		CHECK(em.hasComponent<ComponentTest1>(entity));
	}

	SUBCASE("hasComponent") {
		ska::EntityId entity = 0;
		em.addComponent<ComponentTest1>(entity, ComponentTest1());
		em.removeComponent<ComponentTest1>(entity);
		CHECK(!em.hasComponent<ComponentTest1>(entity));
		em.addComponent<ComponentTest1>(entity, ComponentTest1());
		CHECK(em.hasComponent<ComponentTest1>(entity));
	}

	SUBCASE("removeEntity") {
		ska::EntityId entity = em.createEntity();
		CHECK(entityObserver.events.empty());
		em.addComponent<ComponentTest1>(entity, ComponentTest1());
		
		em.refresh();

		CHECK(componentObserver.eventType.size() == 1);
		CHECK(componentObserver.eventType[0].event == ska::EntityEventTypeEnum::COMPONENT_ALTER);
		CHECK(componentObserver.eventType[0].entityId == entity);

		em.removeEntity(entity);
		CHECK(!em.hasComponent<ComponentTest1>(entity));
		
		em.refresh();

		CHECK(componentObserver.eventType.size() == 2);
		CHECK(componentObserver.eventType[1].event == ska::EntityEventTypeEnum::COMPONENT_ALTER);
		CHECK(componentObserver.eventType[1].entityId == entity);

		CHECK(entityObserver.events.size() == 1);
		CHECK(EntityManagerTestIsEntityRemoved(entityObserver.events[0], entity));

		ska::EntityId entity2 = em.createEntity();
		//Reused deleted entity
		CHECK(entity == entity2);
		CHECK(!em.hasComponent<ComponentTest1>(entity2));
	}

	SUBCASE("removeEntities") {
		auto entity = em.createEntity();
		auto entity2 = em.createEntity();
		auto entityX = em.createEntity();
		
		CHECK(entityObserver.events.size() == 0);

		em.refresh();
		componentObserver.eventType.clear();

		auto setToNotRemove = std::unordered_set<ska::EntityId>{};
		setToNotRemove.insert(entity2);
		em.removeEntities(setToNotRemove);

		em.refresh();

		CHECK(componentObserver.eventType.size() == 2);
		CHECK(componentObserver.eventType[0].event == ska::EntityEventTypeEnum::COMPONENT_ALTER);
		CHECK(componentObserver.eventType[0].entityId == entity);
		CHECK(componentObserver.eventType[1].event == ska::EntityEventTypeEnum::COMPONENT_ALTER);
		CHECK(componentObserver.eventType[1].entityId == entityX);

		CHECK(entityObserver.events.size() == 1);
		CHECK(EntityManagerTestIsEntityRemoved(entityObserver.events[0], entity));
		
		auto entity3 = em.createEntity();

		CHECK(entityX == entity3);
	}

	SUBCASE("Second EntityManager instance : testing the behaviour of component handlers instantiations") {
		ska::EntityId entity = 0;
		auto em2 = ska::EntityManager{ged};
		em2.addComponent<ComponentTest1>(entity, ComponentTest1());
		em2.addComponent<ComponentTest2>(entity, ComponentTest2());
	}
}