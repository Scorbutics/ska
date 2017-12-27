#pragma once
#include "Core/State/State.h"
#include "Core/State/StateBase.h"
#include "ExposeImmutable.h"

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
	public ExposeImmutables<ImmutableStateBaseTracer> {
public:
	MockStateBase() = default;
	virtual ~MockStateBase() = default;

	void beforeLoad(ska::State* s) override { beforeLoadState_ = s; beforeLoadStateDone_ = true; }
	void afterLoad(ska::State* s) override { afterLoadState_ = s; afterLoadStateDone_ = true; }

	bool beforeUnload() override { beforeUnload_ = true; return false; }
	bool afterUnload() override { afterUnload_ = true; return false; }

	void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& dc) override { onGraphicUpdate_ = true; }
	void onEventUpdate(unsigned int ellapsedTime) override { onEventUpdate_ = true; }

	void task(ska::RunnablePtr task) {
		queueTask(std::move(task));
	}
};
