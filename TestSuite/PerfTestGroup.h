#pragma once

#include <string>
#include <vector>
#include <functional>

#include <stdio.h>

#include "Console/ConsoleColor.h"
#include "TestConfig.h"



namespace test {
	struct PerfTest {
		int count;
		long long int sum;
		long long int best;
		long long int worst;
		long long int result;
	};

	class PerfTestGroup {
	public:
		PerfTestGroup(const char* name);

		inline void AddTest(const std::function<void(PerfTestGroup&)>& test);
		inline void Run();

		inline PerfTest& GetMy();
		inline PerfTest& GetOther();

		inline void SetConfig(const TestConfig& config);
		inline TestConfig GetConfig();

	private:
		PerfTest _my;
		PerfTest _other;
		TestConfig _config;
		std::string _name;
		std::vector<std::function<void(PerfTestGroup&)>> _tests;
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	inline PerfTestGroup::PerfTestGroup(const char* name) :
		_name(name) {
	}

	//---------------------------------------------------------------------------
	inline void PerfTestGroup::AddTest(const std::function<void(test::PerfTestGroup&)>& test) {
		_tests.push_back(test);
	}

	//---------------------------------------------------------------------------
	inline void PerfTestGroup::Run() {

		printf("Running unit: %s \n", _name.data());

		for (auto test : _tests) {
			_my = {0};
			_my.best = 99999999999999;
			_other = {0};
			_other.best = 99999999999999;
			test(*this);

			if (_my.result != _other.result) {
				console::SetTextColor(console::RED_TEXT);

				printf("ERROR: results are different check implementation %lld, %lld \n", _my.result, _other.result);
				console::SetTextColor(console::WHITE_TEXT);
			}

			printf("My count %d, Other count: %d \n", _my.count, _other.count);
			if (_my.best > _other.best)
				console::SetTextColor(console::RED_TEXT);
			else
				console::SetTextColor(console::GREEN_TEXT);
			printf("Best:    Perf %lld, other perf %lld, %fx better or worse \n", _my.best, _other.best, _other.best / (double) _my.best);

			if (_my.worst > _other.worst)
				console::SetTextColor(console::RED_TEXT);
			else
				console::SetTextColor(console::GREEN_TEXT);
			printf("Worst:   Perf %lld, other perf %lld, %fx better or worse \n", _my.worst, _other.worst, _other.worst / (double) _my.worst);

			double myavg = _my.sum / (double) _my.count;
			double othavg = _other.sum / (double) _other.count;

			if (myavg > othavg)
				console::SetTextColor(console::RED_TEXT);
			else
				console::SetTextColor(console::GREEN_TEXT);
			printf("Average: Perf %f, other perf %f, %fx better or worse \n \n", myavg, othavg, othavg / myavg);

			console::SetTextColor(console::WHITE_TEXT);
		}

		printf("\nTest group: %s completed \n", _name.data());
	}

	//---------------------------------------------------------------------------
	inline PerfTest& PerfTestGroup::GetMy() {
		return _my;
	}

	//---------------------------------------------------------------------------
	inline PerfTest& PerfTestGroup::GetOther() {
		return _other;
	}

	//---------------------------------------------------------------------------
	inline TestConfig PerfTestGroup::GetConfig() {
		return _config;
	}

	//---------------------------------------------------------------------------
	inline void PerfTestGroup::SetConfig(const TestConfig& config) {
		_config = config;
	}
}
