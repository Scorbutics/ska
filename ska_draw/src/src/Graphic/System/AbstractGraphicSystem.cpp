#include "CameraSystem.h"
#include "AbstractGraphicSystem.h"
#include "Core/Draw/DrawableContainer.h"
#include "../PositionnedGraphicDrawable.h"

ska::AbstractGraphicSystem::AbstractGraphicSystem(ska::CameraSystem& camera) :
	m_camera(camera) {
	m_drawables = nullptr;
}

void ska::AbstractGraphicSystem::setDrawables(ska::DrawableContainer& container) {
	m_drawables = &container;
}

ska::AbstractGraphicSystem::~AbstractGraphicSystem() {
}
