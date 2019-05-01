#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include "GameApp.h"
#include <iostream>

class AppTest : public ska::GameApp {
public:

	explicit AppTest(){}

	virtual int onTerminate(ska::TerminateProcessException&) override{ std::cout << "Terminate process" << std::endl; return -1; };
	virtual int onException(ska::GenericException&) override{ std::cout << "Terminate process" << std::endl; return -1; }

	virtual void run() override{
		doctest::Context context;
		auto resultCode = 0;
		try {

			// sort the test cases by their name
			context.setOption("order-by", "name");
			std::cout << "Running..." << std::endl;
			resultCode = context.run();
		} catch (const std::exception& e) {
			std::cout << "Error : " << e.what() << std::endl;
		}

		if (context.shouldExit() || resultCode != 0) {
			throw ska::TerminateProcessException("Context exit");
		}
	}

	virtual ~AppTest() override {}
};

std::unique_ptr<ska::GameApp> ska::GameApp::get(){
	return std::make_unique<AppTest>();
}


