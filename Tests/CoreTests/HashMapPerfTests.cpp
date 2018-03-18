#include "HashMapPerfTests.h"

#include <TestSuite/PerformanceTest.h>

#include <Core/Allocator/HeapAllocator.h>
#include <Core/Algorithm/Hash.h>
#include <Core/Collection/HashMap.h>
#include <Core/Collection/HashMap.inl>

#include <unordered_map>

#include <TestSuite/Timer.h>

#include <algorithm>    // std::random_shuffle

namespace hashMap_perf_tests {

	/*
		the tests are a bit biased... core::hashmap doesnt do value == value comparison only keys, so the keys have to be unique



		@TODO more tests different KEYS count
	
	*/

	//-----------------------------------------------------------------------------
	void NoHashAdd(test::PerfTestGroup& testGroup) {
		PerformanceTest();
		const unsigned long long keys = 10000;
		const unsigned long long repeat = 20;

		core::Hash** sequence = new core::Hash*[repeat];
		for (int i = 0; i < repeat; ++i) {
			sequence[i] = new core::Hash[keys];
			for (int j = 0; j < keys; ++j) {
				sequence[i][j] = j;
			}
			std::random_shuffle(sequence[i], sequence[i] + keys);
		}
		
		for (int i = 0; i < repeat; ++i) {
			std::unordered_map<core::Hash, int> map;
			PerformanceTestStart(testGroup.GetOther());			
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map[sequence[i][key]] = j * j + j;
			PerformanceTestStop(testGroup.GetOther());
		}

		for (int i = 0; i < repeat; ++i) {
			core::HeapAllocator allocator;
			core::HashMap<int> map;
			map.Init(&allocator);
			PerformanceTestStart(testGroup.GetMy());
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(sequence[i][key], j * j + i);
			PerformanceTestStop(testGroup.GetMy());
		}

		for (int i = 0; i < repeat; ++i) {
			delete[] sequence[i];
		}
		delete[] sequence;
	}

	//-----------------------------------------------------------------------------
	void HashAdd(test::PerfTestGroup& testGroup) {
		PerformanceTest();

		const unsigned long long keys = 10000;
		const unsigned long long repeat = 20;

		core::Hash** sequence = new core::Hash*[repeat];
		for (int i = 0; i < repeat; ++i) {
			sequence[i] = new core::Hash[keys];
			for (int j = 0; j < keys; ++j) {
				sequence[i][j] = j;
			}
			std::random_shuffle(sequence[i], sequence[i] + keys);
		}

		for (int i = 0; i < repeat; ++i) {
			PerformanceTestStart(testGroup.GetOther());
			std::unordered_map<core::Hash, int> map;
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map[sequence[i][key]] = j * j + j;
			PerformanceTestStop(testGroup.GetOther());
		}

		for (int i = 0; i < repeat; ++i) {
			PerformanceTestStart(testGroup.GetMy());
			core::HeapAllocator allocator;
			core::HashMap<int> map;
			map.Init(&allocator);
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::ToHash(&sequence[i][key], sizeof(core::Hash)), j * j + i);
			PerformanceTestStop(testGroup.GetMy());
		}

		for (core::Hash i = 0; i < repeat; ++i) {
			delete[] sequence[i];
		}
		delete[] sequence;
	}

	//-----------------------------------------------------------------------------
	void IterateValuesSum(test::PerfTestGroup& testGroup) {
		PerformanceTest();

		const unsigned long long keys = 10000;
		const unsigned long long repeat = 20;

		core::Hash** sequence = new core::Hash*[repeat];
		for (int i = 0; i < repeat; ++i) {
			sequence[i] = new core::Hash[keys];
			for (int j = 0; j < keys; ++j) {
				sequence[i][j] = j;
			}
			std::random_shuffle(sequence[i], sequence[i] + keys);
		}
		for (int i = 0; i < repeat; ++i) {

			std::unordered_map<core::Hash, int> stdmap;
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				stdmap[sequence[i][key]] = j + j;

			int sum = 0;
			PerformanceTestStart(testGroup.GetOther());
			for (auto& it : stdmap)
				sum += it.second;
			PerformanceTestStop2(testGroup.GetOther(), sum);
		}


		for (int i = 0; i < repeat; ++i) {
			core::HeapAllocator allocator;
			core::HashMap<int> map;
			map.Init(&allocator);
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(sequence[i][key], j + j);

			int sum = 0;
			PerformanceTestStart(testGroup.GetMy());
			for (auto& it : map)
				sum += it;
			PerformanceTestStop2(testGroup.GetMy(), sum);
		}

		for (int i = 0; i < repeat; ++i) {
			delete[] sequence[i];
		}
		delete[] sequence;
	}


	//-----------------------------------------------------------------------------
	void NoHashGet(test::PerfTestGroup& testGroup) {
		PerformanceTest();

		const unsigned long long keys = 10000;
		const unsigned long long repeat = 20;

		core::Hash** sequence = new core::Hash*[repeat];
		for (int i = 0; i < repeat; ++i) {
			sequence[i] = new core::Hash[keys];
			for (int j = 0; j < keys; ++j) {
				sequence[i][j] = j;
			}
			std::random_shuffle(sequence[i], sequence[i] + keys);
		}

		for (int i = 0; i < repeat; ++i) {
			std::unordered_map<core::Hash, int> stdmap;
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				stdmap[sequence[i][key]] = j + j;

			int sum = 0;
			PerformanceTestStart(testGroup.GetOther());
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				sum += stdmap[sequence[i][key]];
			PerformanceTestStop2(testGroup.GetOther(), sum);
		}

		for (int i = 0; i < repeat; ++i) {
			core::HeapAllocator allocator;
			core::HashMap<int> map;
			map.Init(&allocator);
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(sequence[i][key], j + j);

			int sum = 0;
			PerformanceTestStart(testGroup.GetMy());
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				sum += *map.Find(sequence[i][key]);
			PerformanceTestStop2(testGroup.GetMy(), sum);
		}

		for (int i = 0; i < repeat; ++i) {
			delete[] sequence[i];
		}
		delete[] sequence;
	}


	//-----------------------------------------------------------------------------
	void HashGet(test::PerfTestGroup& testGroup) {
		PerformanceTest();

		const unsigned long long keys = 10000;
		const unsigned long long repeat = 20;

		core::Hash** sequence = new core::Hash*[repeat];
		for (int i = 0; i < repeat; ++i) {
			sequence[i] = new core::Hash[keys];
			for (int j = 0; j < keys; ++j) {
				sequence[i][j] = j;
			}
			std::random_shuffle(sequence[i], sequence[i] + keys);
		}

		for (int i = 0; i < repeat; ++i) {
			std::unordered_map<core::Hash, int> stdmap;
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				stdmap[sequence[i][key]] = j + j;

			int sum = 0;
			PerformanceTestStart(testGroup.GetOther());
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				sum += stdmap[sequence[i][key]];
			PerformanceTestStop2(testGroup.GetOther(), sum);
		}

		for (int i = 0; i < repeat; ++i) {
			core::HeapAllocator allocator;
			core::HashMap<int> map;
			map.Init(&allocator);
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::ToHash(&sequence[i][key], sizeof(core::Hash)), j + j);

			int sum = 0;
			PerformanceTestStart(testGroup.GetMy());
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				sum += *map.Find(core::ToHash(&sequence[i][key], sizeof(core::Hash)));
			PerformanceTestStop2(testGroup.GetMy(), sum);
		}

		
		for (int i = 0; i < repeat; ++i) {
			delete[] sequence[i];
		}
		delete[] sequence;

	}

	void FindRemove(test::PerfTestGroup& testGroup) {
		PerformanceTest();

		const unsigned long long keys = 10000;
		const unsigned long long repeat = 20;

		core::Hash** sequence = new core::Hash*[repeat];
		for (int i = 0; i < repeat; ++i) {
			sequence[i] = new core::Hash[keys];
			for (int j = 0; j < keys; ++j) {
				sequence[i][j] = j;
			}
			std::random_shuffle(sequence[i], sequence[i] + keys);
		}

		for (int i = 0; i < repeat; ++i) {
			std::unordered_map<core::Hash, int> stdmap;
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				stdmap[sequence[i][key]] = j + j;

			int sum = 0;
			PerformanceTestStart(testGroup.GetOther());
			for (int key = 0, j = 0; key < keys; ++key, ++j) {
				auto& it = stdmap.find(sequence[i][key]);
				if (it != stdmap.end()) {
					sum += it->second;
					stdmap.erase(it);
				}
			}
			PerformanceTestStop2(testGroup.GetOther(), sum);
		}

		for (int i = 0; i < repeat; ++i) {
			core::HeapAllocator allocator;
			core::HashMap<int> map;
			map.Init(&allocator);
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::ToHash(&sequence[i][key], sizeof(core::Hash)), j + j);

			int sum = 0;
			PerformanceTestStart(testGroup.GetMy());
			for (int key = 0, j = 0; key < keys; ++key, ++j) {
				auto hash = core::ToHash(&sequence[i][key], sizeof(core::Hash));
				auto valuep = map.Find(hash);
				if (valuep) {
					sum += *valuep;
					map.Remove(hash);
				}
			}
			PerformanceTestStop2(testGroup.GetMy(), sum);
		}


		for (int i = 0; i < repeat; ++i) {
			delete[] sequence[i];
		}
		delete[] sequence;

	}

	void FindRemoveHalfInvalid(test::PerfTestGroup& testGroup) {
		PerformanceTest();

		const unsigned long long keys = 10000;
		const unsigned long long repeat = 20;

		core::Hash** sequence = new core::Hash*[repeat];
		for (int i = 0; i < repeat; ++i) {
			sequence[i] = new core::Hash[keys];
			for (int j = 0; j < keys; ++j) {
				sequence[i][j] = j;
			}
			std::random_shuffle(sequence[i], sequence[i] + keys);
		}

		for (int i = 0; i < repeat; ++i) {
			std::unordered_map<core::Hash, int> stdmap;
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				stdmap[sequence[i][key]] = j + j;

			int sum = 0;
			PerformanceTestStart(testGroup.GetOther());
			for (int key = 0, j = 0; key < keys; ++key, ++j) {
				auto k = key % 2 == 0 ? sequence[i][key] : keys + sequence[i][key];
				auto& it = stdmap.find(k);
				if (it != stdmap.end()) {
					sum += it->second;
					stdmap.erase(it);
				}
			}
			PerformanceTestStop2(testGroup.GetOther(), sum);
		}

		for (int i = 0; i < repeat; ++i) {
			core::HeapAllocator allocator;
			core::HashMap<int> map;
			map.Init(&allocator);
			int j = 0;
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::ToHash(&sequence[i][key], sizeof(core::Hash)), j + j);

			int sum = 0;
			PerformanceTestStart(testGroup.GetMy());
			for (int key = 0, j = 0; key < keys; ++key, ++j) {
				auto k = key % 2 == 0 ? sequence[i][key] : keys + sequence[i][key];
				auto hash = core::ToHash(&k, sizeof(core::Hash));
				auto valuep = map.Find(hash);
				if (valuep) {
					sum += *valuep;
					map.Remove(hash);
				}
			}
			PerformanceTestStop2(testGroup.GetMy(), sum);
		}


		for (int i = 0; i < repeat; ++i) {
			delete[] sequence[i];
		}
		delete[] sequence;
	}
}
