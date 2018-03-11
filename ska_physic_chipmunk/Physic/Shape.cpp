#include "Shape.h"
#include "Vect.h"
#include "Body.h"
#include "Rectangle.h"

ska::cp::Shape::Shape() :
	m_shape(nullptr) {
}

ska::cp::Shape ska::cp::Shape::fromSegment(cpBody* body, const Vect& a, const Vect& b, double radius) {
	Shape sh;
	sh.loadFromSegment(body, a, b, radius);
	return sh;
}

ska::cp::Shape ska::cp::Shape::fromCircle(cpBody* body, double radius, const Vect& offset) {
	Shape sh;
	sh.loadFromCircle(body, radius, offset);
	return sh;
}

ska::cp::Shape ska::cp::Shape::fromBox(cpBody* body, const ska::Rectangle& r, double radius) {
	Shape sh;
	sh.loadFromBox(body, r, radius);
	return sh;
}

cpShape* ska::cp::Shape::shape() const {
	return m_shape;
}

ska::Rectangle ska::cp::Shape::getDimensions() const {
	const auto& bb = cpShapeGetBB(m_shape);
	return {static_cast<int>(bb.l), static_cast<int>(bb.t), static_cast<int>(bb.r - bb.l), static_cast<int>(bb.t - bb.b)};
}

ska::cp::Shape::Shape(Shape&& sh) noexcept{
	m_shape = sh.m_shape;
	sh.m_shape = nullptr;
}

ska::cp::Shape& ska::cp::Shape::operator=(Shape&& sh) noexcept{
	std::swap(m_shape, sh.m_shape);
	return *this;
}

ska::cp::Shape::~Shape() {
	free();
}

void ska::cp::Shape::setFriction(float friction) {
	cpShapeSetFriction(m_shape, friction);
}

void ska::cp::Shape::loadFromSegment(cpBody *body, const Vect& a, const Vect& b, double radius) {
	free();
	m_shape = cpSegmentShapeNew(body, a.vect(), b.vect(), radius);
}

void ska::cp::Shape::loadFromBox(cpBody *body, const ska::Rectangle& r, double radius) {
	free();
	const cpBB dBox{ r.x, r.y, r.x + r.w, r.y + r.h };
	m_shape = cpBoxShapeNew2(body, dBox, radius);
}

void ska::cp::Shape::loadFromCircle(cpBody *body, double radius, const Vect& offset) {
	free();
	m_shape = cpCircleShapeNew(body, radius, offset.vect());
}

void ska::cp::Shape::free() {
	cpShapeFree(m_shape);
	m_shape = nullptr;
}

