#pragma once

#include <TestSuite/TestGroup.h>

namespace array_tests {

	void ConstructorSizeTest1(test::TestGroup& testGroup);
	void ConstructorCapacityTest1(test::TestGroup& testGroup);

	void CopyConstructorTest1(test::TestGroup& testGroup);

	void MoveConstructorTest1(test::TestGroup& testGroup);
	void MoveConstructorTest2(test::TestGroup& testGroup);

	void AssignmentOperatorTest1(test::TestGroup& testGroup);

	void MoveAssignmentOperatorTest1(test::TestGroup& testGroup);

	void PushTest1(test::TestGroup& testGroup);
	void PushTest2(test::TestGroup& testGroup);
	void PushMultipleTest1(test::TestGroup& testGroup);

	void TopTest1(test::TestGroup& testGroup);

	void PopTest1(test::TestGroup& testGroup);
	void PopTest2(test::TestGroup& testGroup);
	void PopTest3(test::TestGroup& testGroup);

	void RemoveTest1(test::TestGroup& testGroup);

	void BracketOperatorTest1(test::TestGroup& testGroup);
	void BracketOperatorTest2(test::TestGroup& testGroup);

	void IsEmptyTest1(test::TestGroup& testGroup);

	void CapacityTest1(test::TestGroup& testGroup);

	void CompactTest1(test::TestGroup& testGroup);

	void ClearTest1(test::TestGroup& testGroup);
	void ClearTest2(test::TestGroup& testGroup);

	void FindTest1(test::TestGroup& testGroup);
	void FindTest2(test::TestGroup& testGroup);

	void ReserveTest1(test::TestGroup& testGroup);
	void ReserveTest2(test::TestGroup& testGroup);

	void BeginEndTest1(test::TestGroup& testGroup);
	void CBeginCEndTest1(test::TestGroup& testGroup);

	static test::TestGroup GetTestGroup() {
		using namespace test;

		TestGroup tests("Array tests");

		tests.AddTest(ConstructorSizeTest1);
		tests.AddTest(ConstructorCapacityTest1);
		tests.AddTest(CopyConstructorTest1);
		tests.AddTest(MoveConstructorTest1);
		tests.AddTest(MoveConstructorTest2);
		tests.AddTest(AssignmentOperatorTest1);
		tests.AddTest(MoveAssignmentOperatorTest1);
		tests.AddTest(PushTest1);
		tests.AddTest(PushTest2);
		tests.AddTest(PushMultipleTest1);
		tests.AddTest(TopTest1);
		tests.AddTest(PopTest1);
		tests.AddTest(PopTest2);
		tests.AddTest(PopTest3);
		tests.AddTest(RemoveTest1);
		tests.AddTest(BracketOperatorTest1);
		tests.AddTest(BracketOperatorTest2);
		tests.AddTest(CapacityTest1);
		tests.AddTest(CompactTest1);
		tests.AddTest(ClearTest1);
		tests.AddTest(ClearTest2);
		tests.AddTest(FindTest1);
		tests.AddTest(FindTest2);
		tests.AddTest(ReserveTest1);
		tests.AddTest(ReserveTest2);
		tests.AddTest(BeginEndTest1);
		tests.AddTest(CBeginCEndTest1);

	

		return tests;
	}
}







