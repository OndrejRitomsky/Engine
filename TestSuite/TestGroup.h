#pragma once

#include <string>
#include <vector>
#include <functional>

#include <stdio.h>

#include "Console/ConsoleColor.h"
#include "TestConfig.h"



namespace test
{
	class TestGroup {
	public:
		TestGroup(const char* name);

		inline void AddTest(const std::function<void(TestGroup&)>& test);
		inline void Run();

    inline void ResetTimes();

    inline void IncTimes();
		inline void IncTested();
		inline void IncCorrects();

    inline unsigned int GetTimes();
		inline unsigned int GetTested();
		inline unsigned int GetCorrects();

		inline void SetConfig(const TestConfig& config);
		inline TestConfig GetConfig();

	private:
    unsigned int _times;
		unsigned int _tested;
		unsigned int _corrects;
		TestConfig _config;
		std::string _name;
		std::vector<std::function<void(TestGroup&)>> _tests;
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	inline TestGroup::TestGroup(const char* name) :
		_name(name),
		_tested(0),
		_corrects(0) {
	}

	//---------------------------------------------------------------------------
	inline void TestGroup::AddTest(const std::function<void(test::TestGroup&)>& test) {
		_tests.push_back(test);
	}

	//---------------------------------------------------------------------------
	inline void TestGroup::Run() {
		_tested = 0;
		_corrects = 0;
		printf("Running unit: %s \n", _name.data());

		for (auto test : _tests) {
			test(*this);
		}

		printf("Test group: %s completed \n", _name.data());
		float ratio = _tested == 0 ? 0 : (float) _corrects / _tested;
		ratio *= 100;
		if (_corrects == _tested)
			console::SetTextColor(console::GREEN_TEXT);
		else
			console::SetTextColor(console::RED_TEXT);

		printf("Test group results %d / %d, %f%% \n\n", _corrects, _tested, ratio);

		console::SetTextColor(console::WHITE_TEXT);
	}

  inline void TestGroup::ResetTimes() {
    _times = 0;
  }

  inline void TestGroup::IncTimes() {
    _times++;
  }

	//---------------------------------------------------------------------------
	inline void TestGroup::IncTested() {
		_tested++;
	}

	//---------------------------------------------------------------------------
	inline void TestGroup::IncCorrects() {
		_corrects++;
	}

  //---------------------------------------------------------------------------
  inline unsigned int TestGroup::GetTimes() {
    return _times;
  }

	//---------------------------------------------------------------------------
	inline unsigned int TestGroup::GetTested() {
		return _tested;
	}

	//---------------------------------------------------------------------------
	inline unsigned int TestGroup::GetCorrects() {
		return _corrects;
	}

	//---------------------------------------------------------------------------
	inline TestConfig TestGroup::GetConfig() {
		return _config;
	}

	//---------------------------------------------------------------------------
	inline void TestGroup::SetConfig(const TestConfig& config) {
		_config = config;
	}
}
