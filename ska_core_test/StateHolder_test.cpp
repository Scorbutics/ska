#include <doctest.h>
#include "Core/State/StateHolder.h"
#include "Draw/VectorDrawableContainer.h"
#include "Draw/Renderer.h"
#include "Utils/SubObserver.h"
#include "Exceptions/StateDiedException.h"

class StateListener :
	public ska::SubObserver<ska::StateEvent> {
public:
	explicit StateListener(ska::GameEventDispatcher& ged) :
		SubObserver<ska::StateEvent>(std::bind(&StateListener::onStateEvent, this, std::placeholders::_1), ged){}

	const ska::StateEvent* getLastStateEventTriggered() const{
		return m_lastStateEvent.get();
	}

private:
	bool onStateEvent(ska::StateEvent& se){
		m_lastStateEvent = std::make_unique<ska::StateEvent>(se.type);
		return false;
	}

	std::unique_ptr<ska::StateEvent> m_lastStateEvent;
};

class MockRenderer : public ska::Renderer {
public:
	SDL_Texture* createTextureFromSurface(const ska::SDLSurface& surface) const override{ return 0; }

	void render(const ska::Texture& t, int posX, int posY, ska::Rectangle const*) const override{}
	void render(const ska::AnimatedTexture& t, int posX, int posY, ska::Rectangle const*) const override{}
	void update() const override{}
	void drawColorPoint(const ska::Color& c, const ska::Point<int>& pos) const override{}
	void drawColorRect(const ska::Color& c, const ska::Rectangle& r) const override{}
	void drawColorLine(const ska::Color& c, const ska::Point<int>& p1, const ska::Point<int>& p2) const override{}
};

class MockState : public ska::State {
public:
	explicit MockState(ska::StateHolder& sh) : ska::State(sh){}
	~MockState() = default;

	void graphicUpdate(unsigned ellapsedTime, ska::DrawableContainer& drawables) override{}
	void eventUpdate(unsigned) override{}

	void load(std::unique_ptr<State>* lastState) override{}
	bool unload() override{ return false; }

private:

};

TEST_CASE("[StateHolder] "){
	ska::GameEventDispatcher ged;

	SUBCASE("First state loaded") {
		ska::StateHolder stateHolder(ged);
		StateListener stateListener(ged);

		CHECK(stateListener.getLastStateEventTriggered() == nullptr);
		
		//First state
		stateHolder.nextState(std::make_unique<MockState>(stateHolder));
		stateHolder.update();

		auto lastEvent = stateListener.getLastStateEventTriggered();
		CHECK(lastEvent != nullptr);
		CHECK(lastEvent->type == ska::FIRST_STATE_LOADED);
	}

	SUBCASE("State change") {
		ska::StateHolder stateHolder(ged);
		StateListener stateListener(ged);

		//First state
		stateHolder.nextState(std::make_unique<MockState>(stateHolder));
		stateHolder.update();

		//Next state
		stateHolder.nextState(std::make_unique<MockState>(stateHolder));
		try {
			stateHolder.update();
			CHECK(false);
		} catch (ska::StateDiedException& sde) {
			auto lastEvent = stateListener.getLastStateEventTriggered();
			CHECK(lastEvent->type == ska::STATE_CHANGED);
		}
	}
}
