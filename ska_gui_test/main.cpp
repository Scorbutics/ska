#undef main
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include "GameApp.h"
#include <iostream>

class AppTest : public ska::GameApp {
public:

	explicit AppTest(ska::GameConfiguration&& gc)
		: ska::GameApp(std::forward<ska::GameConfiguration>(gc)) {
	}

	virtual int onTerminate(ska::TerminateProcessException&) { std::cout << "Terminate process" << std::endl; return -1; };
	virtual int onException(ska::GenericException&) { std::cout << "Terminate process" << std::endl; return -1; }

	virtual void run() {
		doctest::Context context;
		try {
			
			// sort the test cases by their name
			context.setOption("order-by", "name");
			std::cout << "Running..." << std::endl;
			context.run();
		} catch (const std::exception& e) {
			std::cout << "Error : " << e.what() << std::endl;
		}

		if (context.shouldExit()) {
			throw ska::TerminateProcessException("Context exit");
		}
	}

	virtual ~AppTest() override {}
};

std::unique_ptr<ska::GameApp> ska::GameApp::get(){
	ska::GameConfiguration gc;
	return std::make_unique<AppTest>(std::move(gc));
}


