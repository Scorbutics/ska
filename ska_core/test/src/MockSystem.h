#pragma once
#include "ECS/ISystem.h"
#include "ExposeImmutable.h"

struct ImmutableSystemTracer {
	ImmutableSystemTracer() : update_(false) {}
	bool update_;
};

class MockSystem :
	public ska::ISystem,
	public ExposeImmutable<ImmutableSystemTracer> {

public:
	void update(unsigned ellapsedTime) override { update_ = true; if (call_) { call_(); } }
	void whenUpdate(std::function<void(void)> call){
		call_ = call;
	}
private:
	std::function<void(void)> call_;
};