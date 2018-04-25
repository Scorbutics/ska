#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <iostream>
#include "GameApp.h"

class AppTest : 
	public ska::GameApp {

public:
	explicit AppTest() = default;

	int onTerminate(ska::TerminateProcessException&) override { std::cout << "Terminate process" << std::endl; return -1; };
	int onException(ska::GenericException&) override { std::cout << "Terminate process" << std::endl; return -1; }

	void run() override {
		doctest::Context context;
		auto resultCode = 0;
		try {

			// sort the test cases by their name
			context.setOption("order-by", "name");
			context.setOption("success", "true");
			std::cout << "Running..." << std::endl;
			resultCode = context.run();
		}
		catch (const std::exception& e) {
			std::cout << "Error : " << e.what() << std::endl;
		}

		if (context.shouldExit() || resultCode != 0) {
			throw ska::TerminateProcessException("Context exit");
		}
	}

	~AppTest() override = default;
};

std::unique_ptr<ska::GameApp> ska::GameApp::get() {
	return std::make_unique<AppTest>();
}


