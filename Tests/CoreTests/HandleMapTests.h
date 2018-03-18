#pragma once

#include <TestSuite/TestGroup.h>

namespace handleMap_tests {

	void ConstructorSizeTest1(test::TestGroup& testGroup);
	void ConstructorCapacityTest1(test::TestGroup& testGroup);

	void AddTest1(test::TestGroup& testGroup);
	void GetTest1(test::TestGroup& testGroup);
	void RemoveTest1(test::TestGroup& testGroup);

	void ReserveTest1(test::TestGroup& testGroup);
	void ReserveTest2(test::TestGroup& testGroup);

	void BeginEndTest1(test::TestGroup& testGroup);
	void CBeginCEndTest1(test::TestGroup& testGroup);

	void MultipleTest1(test::TestGroup& testGroup);
	void MultipleTest2(test::TestGroup& testGroup);

	static test::TestGroup GetTestGroup() {
		using namespace test;

		TestGroup tests("Handle map tests");

		tests.AddTest(ConstructorSizeTest1);
		tests.AddTest(ConstructorCapacityTest1);

		tests.AddTest(AddTest1);
		tests.AddTest(GetTest1);
		tests.AddTest(RemoveTest1);
		
		tests.AddTest(ReserveTest1);
		tests.AddTest(ReserveTest2);
		tests.AddTest(BeginEndTest1);
		tests.AddTest(CBeginCEndTest1);

		tests.AddTest(MultipleTest1);
		tests.AddTest(MultipleTest2);

		return tests;
	}
}


