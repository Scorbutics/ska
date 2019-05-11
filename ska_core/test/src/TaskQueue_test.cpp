#include <doctest.h>
#include "Base/Task/TaskQueue.h"

TEST_CASE("[TaskQueue]"){
	ska::TaskQueue taskQueue;
	
	SUBCASE("Has 1 running task") {
		auto callCount = 0;
		taskQueue.queueTask([&]() {
			return callCount++ != 1;
		});

		taskQueue.refresh();
		CHECK(callCount == 1);
		CHECK(taskQueue.hasRunningTask());
		
		taskQueue.refresh();
		CHECK(callCount == 2);
		CHECK(!taskQueue.hasRunningTask());
	}

	SUBCASE("Has several running tasks") {
		auto callCount = 0;
		taskQueue.queueTask([&]() {
			return callCount++ != 0;
		});

		taskQueue.queueTask([&]() {
			return callCount++ != 2;
		});

		taskQueue.refresh();
		CHECK(callCount == 1);
		CHECK(taskQueue.hasRunningTask());

		taskQueue.refresh();
		CHECK(callCount == 2);
		CHECK(taskQueue.hasRunningTask());

		taskQueue.refresh();
		CHECK(callCount == 3);
		CHECK(!taskQueue.hasRunningTask());
	}
}



