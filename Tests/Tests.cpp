
#include "CoreTests/CoreTests.h"

#include <TestSuite/TestGroupCollection.h>

int main()
{

	using namespace test;
	
	TestConfig testConfig;
	testConfig.logSuccess = true;
	testConfig.stopOnErorr = false;
	testConfig.stopOnEnd = true;

	TestGroupCollection defaultCollection = CoreTestGroupDefaultCollection(testConfig);
	defaultCollection.Run();


  PerfTestGroupCollection perfCollection = CorePerformanceTestGroupDefaultCollection(testConfig);
	perfCollection.Run();


	return 0;
}

