#pragma once
#include "Core/State/State.h"
#include "Core/State/StateBase.h"
#include "ExposeImmutable.h"
#include "MockGraphicSystem.h"
#include "MockSystem.h"

struct ImmutableStateBaseTracer {
	ImmutableStateBaseTracer() : beforeLoadState_(nullptr),
		afterLoadState_(nullptr),
		beforeLoadStateDone_(false),
		afterLoadStateDone_(false),
		beforeUnload_(false),
		afterUnload_(false),
		onGraphicUpdate_(false),
		onEventUpdate_(false) {}

	ska::State* beforeLoadState_;
	ska::State* afterLoadState_;

	bool beforeLoadStateDone_;
	bool afterLoadStateDone_;

	bool beforeUnload_;
	bool afterUnload_;

	bool onGraphicUpdate_;
	bool onEventUpdate_;
};

class MockStateBase : 
	public ska::StateBase, 
	public ExposeImmutable<ImmutableStateBaseTracer> {
public:
	MockStateBase() = default;
	virtual ~MockStateBase() = default;

	void beforeLoad(ska::State* s) override { beforeLoadState_ = s; beforeLoadStateDone_ = true; }
	void afterLoad(ska::State* s) override { afterLoadState_ = s; afterLoadStateDone_ = true; }

	void beforeUnload() override { beforeUnload_ = true; }
	void afterUnload() override { afterUnload_ = true; }

	void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& dc) override { onGraphicUpdate_ = true; }
	void onEventUpdate(unsigned int ellapsedTime) override { onEventUpdate_ = true; }

	void task(ska::RunnablePtr task) {
		queueTask(std::move(task));
	}

	MockGraphicSystem& addMockGraphicSystem() {
		auto ptr = std::make_unique<MockGraphicSystem>();
		auto& result = *ptr.get();
		addGraphic(std::move(ptr));
		return result;
	}

	MockGraphicSystem& addPriorizedMockGraphicSystem(unsigned int priority) {
		auto ptr = std::make_unique<MockGraphicSystem>();
		auto& result = *ptr.get();
		addPriorizedGraphic(priority, std::move(ptr));
		return result;
	}

	MockSystem& addMockSystem() {
		auto ptr = std::make_unique<MockSystem>();
		auto& result = *ptr.get();
		addLogic(std::move(ptr));
		return result;
	}

	MockSystem& addPriorizedMockSystem(unsigned int priority) {
		auto ptr = std::make_unique<MockSystem>();
		auto& result = *ptr.get();
		addPriorizedLogic(priority, std::move(ptr));
		return result;
	}
};
