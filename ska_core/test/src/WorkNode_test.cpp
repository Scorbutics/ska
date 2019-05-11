#include <doctest.h>
#include "Base/Task/WorkNode.h"

TEST_CASE("[WorkNode] wn "){
	class Dummy {

	};
	Dummy dummy;
	ska::WorkNode<Dummy> wn;
	
	auto called = 0;
	wn.then([&] (Dummy& dummy) {
		called++;
	});
	
	CHECK(called == 0);

	wn.executeAndPop(dummy);

	CHECK(called == 1);

	wn.executeAndPop(dummy);

	CHECK(called == 1);
}