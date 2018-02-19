#include "Shape.h"
#include "Vect.h"

ska::cp::Shape::Shape() :
	m_shape(nullptr) {
}

ska::cp::Shape&& ska::cp::Shape::fromSegment(cpBody* body, const Vect& a, const Vect& b, float radius) {
	Shape sh;
	sh.loadFromSegment(body, a, b, radius);
	return std::move(sh);
}

ska::cp::Shape&& ska::cp::Shape::fromCircle(cpBody* body, float radius, const Vect& offset) {
	Shape sh;
	sh.loadFromCircle(body, radius, offset);
	return std::move(sh);
}

cpShape* ska::cp::Shape::shape() const {
	return m_shape;
}

ska::cp::Shape::~Shape() {
	free();
}

void ska::cp::Shape::setFriction(float friction) {
	cpShapeSetFriction(m_shape, friction);
}

void ska::cp::Shape::loadFromSegment(cpBody *body, const Vect& a, const Vect& b, float radius) {
	free();
	m_shape = cpSegmentShapeNew(body, a.vect(), b.vect(), radius);
}

void ska::cp::Shape::loadFromCircle(cpBody *body, float radius, const Vect& offset) {
	free();
	m_shape = cpCircleShapeNew(body, radius, offset.vect());
}

void ska::cp::Shape::free() {
	cpShapeFree(m_shape);
	m_shape = nullptr;
}

