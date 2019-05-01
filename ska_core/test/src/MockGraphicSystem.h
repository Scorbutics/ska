#pragma once
#include "Draw/DrawableContainer.h"
#include "Draw/IGraphicSystem.h"
#include "ExposeImmutable.h"

namespace ska {
	class CameraSystem;
}

struct ImmutableGraphicSystemTracer {
	ImmutableGraphicSystemTracer() :
		update_(false),
		linkCamera_(nullptr),
		linkCameraDone_(false),
		drawables_(nullptr),
		drawablesDone_(false) {}

	bool update_;
	ska::CameraSystem* linkCamera_;
	bool linkCameraDone_;
	ska::DrawableContainer* drawables_;
	bool drawablesDone_;
};

class MockGraphicSystem :
	public ska::IGraphicSystem,
	public ExposeImmutable<ImmutableGraphicSystemTracer> {

public:
	void update(unsigned ellapsedTime) override { update_ = true; if(call_){call_(); } }
	void setDrawables(ska::DrawableContainer& container) override { drawables_ = &container; drawablesDone_ = true; }
	void whenUpdate(std::function<void(void)> p) {
		call_ = p;
	}

private:
	std::function<void(void)> call_;
};

