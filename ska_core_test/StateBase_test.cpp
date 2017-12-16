#include <doctest.h>
#include <fakeit.hpp>
#include "Core/State/StateBase.h"
#include "ECS/EntityManager.h"
#include "Draw/VectorDrawableContainer.h"
#include "Draw/Renderer.h"

/*class StateBaseTest : 
	public ska::StateBase<ska::EntityManager, ska::GameEventDispatcher> {
public:
	StateBaseTest(ska::EntityManager& em, ska::GameEventDispatcher& ged, ska::StateHolder& sh) : 
		ska::StateBase<ska::EntityManager, ska::GameEventDispatcher>(em, ged, sh) {}

	virtual ~StateBaseTest() = default;
	
};*/

TEST_CASE("[StateBase]") {
	using namespace fakeit;

	ska::GameEventDispatcher ged;
	ska::EntityManager em(ged);
	ska::StateHolder sh(ged);

	Mock<ska::Renderer> renderer;
	ska::VectorDrawableContainer drawableContainer(renderer.get());

	SUBCASE("graphicUpdate") {
		/*StateBaseTest sbt(em, ged, sh);
		sbt.addSubState<StateBaseTest>();

		sbt.graphicUpdate(0, drawableContainer);*/
	}
}