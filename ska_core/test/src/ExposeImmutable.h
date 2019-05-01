#pragma once

template<class ToExpose>
class ExposeImmutable : protected ToExpose {
public:
	ExposeImmutable() : read(*this) {}
	virtual ~ExposeImmutable() = default;

	const ToExpose& read;
};

template <class ... PODs>
struct AggregatePOD : public PODs ... {};

template <class ... ToExpose>
class ExposeImmutables : public ExposeImmutable<AggregatePOD<ToExpose...>> {};