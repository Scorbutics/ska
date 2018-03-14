#include "Constraint.h"
#include "Physic/Vect.h"

ska::cp::Constraint::Constraint() :
	m_joint(nullptr) {
	
}

ska::cp::Constraint::Constraint(Constraint&& c) noexcept {
	m_joint = c.m_joint;
	c.m_joint = nullptr;
}

ska::cp::Constraint& ska::cp::Constraint::operator=(Constraint&& c) noexcept {
	std::swap(m_joint, c.m_joint);
	return *this;
}

ska::cp::Constraint::Constraint(cpBody& body1, cpBody& body2, const Vect& anchor1, const Vect& anchor2) :
	m_joint(cpPivotJointNew2(&body1, &body2, anchor1.vect(), anchor2.vect())) {
}

ska::cp::Constraint::Constraint(cpBody& body1, cpBody& body2, double phase, double ratio) :
	m_joint(cpGearJointNew(&body1, &body2, phase, ratio)) {
}

void ska::cp::Constraint::free() {
	cpConstraintFree(m_joint);
}

ska::cp::Constraint::~Constraint() {
	free();
}

cpConstraint* ska::cp::Constraint::constraint() {
	return m_joint;
}

ska::cp::Constraint ska::cp::Constraint::buildGearJoint(cpBody& body1, cpBody& body2, double phase, double ratio){
	return Constraint(body1, body2, phase, ratio);
}

ska::cp::Constraint ska::cp::Constraint::buildPivotJoint(cpBody& body1, cpBody& body2, const Vect& anchor1, const Vect& anchor2) {
	return Constraint(body1, body2, anchor1, anchor2);
}

void ska::cp::Constraint::setMaxForce(double maxForce) {
	cpConstraintSetMaxForce(m_joint, maxForce);
}

void ska::cp::Constraint::setMaxBias(double maxBias) {
	cpConstraintSetMaxBias(m_joint, maxBias);
}
