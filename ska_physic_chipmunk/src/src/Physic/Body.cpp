#include "Body.h"
#include "Vect.h"

ska::cp::Body::Body() :
	m_body(nullptr) {
}

ska::cp::Body::Body(Body&& b) noexcept{
	m_body = b.m_body;
	setEntity(b.m_entity);

	m_constraints = std::move(b.m_constraints);
	m_shapes = std::move(b.m_shapes);

	b.m_body = nullptr;
	b.m_entity = 0;
}

ska::cp::Body& ska::cp::Body::operator=(Body&& b) noexcept{
	std::swap(m_body, b.m_body);
	std::swap(m_entity, b.m_entity);
	std::swap(m_constraints, b.m_constraints);
	std::swap(m_shapes, b.m_shapes);
	return *this;
}

ska::cp::Body::~Body() {
	free();
}

cpBody& ska::cp::Body::body() {
	return *m_body;
}

const cpBody& ska::cp::Body::body() const {
	return *m_body;
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

void ska::cp::Body::setEntity(EntityId entity) {
	m_entity = entity;
	cpBodySetUserData(m_body, &m_entity);
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

ska::cp::Body ska::cp::Body::fromMassAndSizeForBox(double mass, unsigned int w, unsigned int h) {
	Body body;
	body.loadBoxFromMassAndSize(mass, w, h);
	return body;
}

ska::cp::Body ska::cp::Body::fromKinematic() {
	auto body = Body{};
	body.loadFromKinematic();
	return body;
}

void ska::cp::Body::linkConstraint(const std::size_t constraintIndex) {
	m_constraints.push_back(constraintIndex);
}

void ska::cp::Body::linkShape(const std::size_t shapeIndex) {
	m_shapes.push_back(shapeIndex);
}

cpVect ska::cp::Body::getPosition() const {
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

const std::vector<std::size_t>& ska::cp::Body::getConstraints() const {
	return m_constraints;
}

const std::vector<std::size_t>& ska::cp::Body::getShapes() const {
	return m_shapes;
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

void ska::cp::Body::loadBoxFromMassAndSize(double mass, unsigned int w, unsigned int h) {
	loadFromMoment(mass, cpMomentForBox(mass, w, h));
}

void ska::cp::Body::loadFromMoment(double mass, double moment) {
	free();
	m_body = cpBodyNew(mass, moment);
}

void ska::cp::Body::loadFromRadius(double mass, double radius) {
	const auto moment = cpMomentForCircle(mass, 0, radius, cpvzero);
	loadFromMoment(mass, moment);
}

void ska::cp::Body::loadFromKinematic() {
	free();
	m_body = cpBodyNewKinematic();
}

void ska::cp::Body::free() {
	cpBodyFree(m_body);
	m_body = nullptr;
}


