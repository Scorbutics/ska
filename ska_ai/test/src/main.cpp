#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <iostream>

int main(int argc, char* argv[]) {
	doctest::Context context;
	auto resultCode = 0;

	// sort the test cases by their name
	context.setOption("order-by", "name");
	context.setOption("success", "true");
	resultCode = context.run();

	if (context.shouldExit() || resultCode != 0) {
		return(-1);
	}

	return resultCode;
}


