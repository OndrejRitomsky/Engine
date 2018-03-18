#pragma once

#include "CoreTests/HandleMapTests.h"
#include "CoreTests/HashMapTests.h"
#include "CoreTests/ArrayTests.h"

#include "CoreTests/HashMapPerfTests.h"


#include <TestSuite/PerfTestGroupCollection.h>
#include <TestSuite/TestGroupCollection.h>

static test::TestGroupCollection CoreTestGroupDefaultCollection(TestConfig testConfig) {
	using namespace test;

	test::TestGroupCollection col("Core", testConfig);


	col.AddTestGroup(array_tests::GetTestGroup());
	col.AddTestGroup(handleMap_tests::GetTestGroup());
	col.AddTestGroup(hashMap_tests::GetTestGroup());

	return col;

}

static test::PerfTestGroupCollection CorePerformanceTestGroupDefaultCollection(TestConfig testConfig) {
  using namespace test;

  test::PerfTestGroupCollection col("Core Performance", testConfig);

  col.AddTestGroup(hashMap_perf_tests::GetPerfTestGroup());

  return col;

}



