#pragma once

#include <TestSuite/TestGroup.h>


namespace hashMap_tests {

	void ConstructorSizeTest1(test::TestGroup& testGroup);

	void AddTest1(test::TestGroup& testGroup);
	void FindTest1(test::TestGroup& testGroup);
	void FindTest2(test::TestGroup& testGroup);
	void RemoveTest1(test::TestGroup& testGroup);
	void RemoveTest2(test::TestGroup& testGroup);

	void Iterator(test::TestGroup& testGroup);
	void ConstIterator(test::TestGroup& testGroup);
  void ConstKeyValueIterator(test::TestGroup& testGroup);

	void MultipleTest1(test::TestGroup& testGroup);
	void MultipleTest2(test::TestGroup& testGroup);

	static test::TestGroup GetTestGroup() {
		using namespace test;

		TestGroup tests("Hash map tests");

		tests.AddTest(ConstructorSizeTest1);
		tests.AddTest(AddTest1);
		tests.AddTest(FindTest1);
		tests.AddTest(FindTest2);
		tests.AddTest(RemoveTest1);
		tests.AddTest(RemoveTest2);
		tests.AddTest(Iterator);
		tests.AddTest(ConstIterator);
    tests.AddTest(ConstKeyValueIterator);

		tests.AddTest(MultipleTest1);
		tests.AddTest(MultipleTest2);
		return tests;
	}
}


