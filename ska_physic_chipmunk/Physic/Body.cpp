#include "Body.h"
#include "Vect.h"

ska::cp::Body::Body() :
	m_body(nullptr) {
}

ska::cp::Body::Body(Body&& b) noexcept{
	m_body = b.m_body;
	b.m_body = nullptr;
}

ska::cp::Body& ska::cp::Body::operator=(Body&& b) noexcept{
	std::swap(m_body, b.m_body);
	return *this;
}

ska::cp::Body::~Body() {
	free();
}

cpBody* ska::cp::Body::body() const {
	return m_body;
}

cpVect ska::cp::Body::getRotation() const {
	return cpBodyGetRotation(m_body);
}

void ska::cp::Body::applyForce(const Vect& force) {
	cpBodyApplyForceAtLocalPoint(m_body, force.vect(), cpvzero);
}

void ska::cp::Body::applyImpulse(const Vect& impulse) {
	cpBodyApplyImpulseAtLocalPoint(m_body, impulse.vect(), cpvzero);
}

ska::cp::Body ska::cp::Body::fromMoment(double mass, double moment) {
	Body body;
	body.loadFromMoment(mass, moment);
	return body;
}

ska::cp::Body ska::cp::Body::fromRadius(double mass, double radius) {
	Body body;
	body.loadFromRadius(mass, radius);
	return body;
}

cpVect ska::cp::Body::getPosition() const{
	return cpBodyGetPosition(m_body);
}

cpVect ska::cp::Body::getVelocity() const {
	return cpBodyGetVelocity(m_body);
}

void ska::cp::Body::setPosition(const Vect& p) {
	cpBodySetPosition(m_body, p.vect());
}

void ska::cp::Body::setVelocity(const Vect& v) {
	cpBodySetVelocity(m_body, v.vect());
}

void ska::cp::Body::setType(BodyType type) {
	switch (type) {
	case BodyType::STATIC:
		cpBodySetType(m_body, CP_BODY_TYPE_STATIC);
	break;
	case BodyType::KINEMATIC:
		cpBodySetType(m_body, CP_BODY_TYPE_KINEMATIC);
		break;
	case BodyType::DYNAMIC:
		cpBodySetType(m_body, CP_BODY_TYPE_DYNAMIC);
		break;
	default:
		break;
	}
}

void ska::cp::Body::loadFromMoment(double mass, double moment) {
	free();
	m_body = cpBodyNew(mass, moment);
}

void ska::cp::Body::loadFromRadius(double mass, double radius) {
	const auto moment = cpMomentForCircle(mass, 0, radius, cpvzero);
	loadFromMoment(mass, moment);
}

void ska::cp::Body::free() {
	cpBodyFree(m_body);
	m_body = nullptr;
}


