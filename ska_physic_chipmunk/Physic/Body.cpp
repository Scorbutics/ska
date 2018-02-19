#include "Body.h"
#include "Vect.h"

ska::cp::Body::Body() :
	m_body(nullptr) {
}

ska::cp::Body::~Body() {
	free();
}

cpBody* ska::cp::Body::body() const {
	return m_body;
}

ska::cp::Body&& ska::cp::Body::fromMoment(double mass, double moment) {
	Body body;
	body.loadFromMoment(mass, moment);
	return std::move(body);
}

ska::cp::Body&& ska::cp::Body::fromRadius(double mass, double radius) {
	Body body;
	body.loadFromRadius(mass, radius);
	return std::move(body);
}

cpVect ska::cp::Body::getPosition() const{
	return cpBodyGetPosition(m_body);
}

cpVect ska::cp::Body::getVelocity() const {
	return cpBodyGetVelocity(m_body);
}

void ska::cp::Body::setPosition(const Vect& p) const {
	cpBodySetPosition(m_body, p.vect());
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


