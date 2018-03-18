#pragma once

#include "Test.h"
#include "TestGroup.h"
#include "TestConfig.h"

#include "Console/ConsoleColor.h"
#include <string>
#include <vector>

namespace test
{

	class TestGroupCollection {
	public:
		TestGroupCollection(const char* name, const TestConfig& config);

		void AddTestGroup(TestGroup& testGroup);
		void Run();

	private:
		unsigned int _tested;
		unsigned int _corrects; 
		TestConfig _config;
		std::string _name;
		std::vector<TestGroup> _testUnits;
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	TestGroupCollection::TestGroupCollection(const char* name, const TestConfig& config) :
		_name(name),
		_tested(0),
		_corrects(0),
		_config(config) {
	}

	//---------------------------------------------------------------------------
	void TestGroupCollection::AddTestGroup(TestGroup& testGroup) {
		_testUnits.push_back(testGroup);
	}

	//---------------------------------------------------------------------------
	void TestGroupCollection::Run() {
		console::SetTextColor(console::WHITE_TEXT);

		_tested = 0;
		_corrects = 0;

		size_t size = _testUnits.size();

		printf("Running group collection: %s \n \n", _name.data());

		for (auto testUnit : _testUnits) {
			testUnit.SetConfig(_config);
			testUnit.Run();
			_tested += testUnit.GetTested();
			_corrects += testUnit.GetCorrects();
		}

		printf("Test group collection: %s completed \n", _name.data());
    float ratio = _tested == 0 ? 0 : (float) _corrects / _tested;
		ratio *= 100;
		if (_corrects == _tested)
			console::SetTextColor(console::GREEN_TEXT);
		else
			console::SetTextColor(console::RED_TEXT);

		printf("Tests results %d / %d, %f%% \n \n", _corrects, _tested, ratio);

		console::SetTextColor(console::WHITE_TEXT);

		if (_config.stopOnEnd) {
			char c;
			scanf_s("%c", &c, 1u);
		}
	}

}
