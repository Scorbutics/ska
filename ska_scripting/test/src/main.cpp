#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <iostream>

int main(int argc, char* argv[]) {
	doctest::Context context = doctest::Context{argc, argv};
	context.setOption("order-by", "name");
	const auto resultCode = context.run();
	if (context.shouldExit() || resultCode != 0) {
		return -1;
	}
	return resultCode;
}
