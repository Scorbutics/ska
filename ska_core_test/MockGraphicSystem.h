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
	void update(unsigned ellapsedTime) override { update_ = true; }
	void linkCamera(ska::CameraSystem* cs) override { linkCamera_ = cs; linkCameraDone_ = true; }
	void setDrawables(ska::DrawableContainer& container) override { drawables_ = &container; drawablesDone_ = true; }
};