#pragma once

#include <TestSuite/PerfTestGroup.h>


namespace hashMap_perf_tests {



  void NoHashAdd(test::PerfTestGroup& testGroup);
  void HashAdd(test::PerfTestGroup& testGroup);
  void IterateValuesSum(test::PerfTestGroup& testGroup);
	void NoHashGet(test::PerfTestGroup& testGroup);
	void HashGet(test::PerfTestGroup& testGroup);

	void FindRemove(test::PerfTestGroup& testGroup);
	void FindRemoveHalfInvalid(test::PerfTestGroup& testGroup);


  static test::PerfTestGroup GetPerfTestGroup() {
    using namespace test;

    PerfTestGroup tests("Hash map performance tests map tests");

    tests.AddTest(NoHashAdd);
		tests.AddTest(HashAdd);
    tests.AddTest(IterateValuesSum);
		tests.AddTest(NoHashGet);
		tests.AddTest(HashGet);
		tests.AddTest(FindRemove);
		tests.AddTest(FindRemoveHalfInvalid);

    return tests;
  }
}


