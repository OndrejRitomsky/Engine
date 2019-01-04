#pragma once

#include "../test.h"

#include <core/algorithm/hash.h>
#include <core/collection/hashmap.h>
#include <core/collection/hashmap.inl>

#include <unordered_map>

#include <algorithm>    // std::random_shuffle


#include "../flush.h"

#include "core/allocator/allocators.h"

#define DEFINE_ALLOCATOR() \
	core::HeapAllocator heapAll; \
	core::IAllocator* allocator = HeapAllocatorInit(&heapAll); 

#define DEFINE_HASHMAP(type, name) \
	core::HashMap<type> name; \
	name.Init(allocator);

//the tests are a bit biased... h64map doesnt do value == value comparison only keys, so the keys have to be unique
//@TODO more tests different KEYS count



TEST_PERF(HashMapNoHashAdd) {
	const unsigned long long keys = 10000;
	const unsigned long long repeat = 20;

	h64** sequence = new h64*[repeat];
	for (int i = 0; i < repeat; ++i) {
		sequence[i] = new h64[keys];
		for (int j = 0; j < keys; ++j) {
			sequence[i][j] = j;
		}
		std::random_shuffle(sequence[i], sequence[i] + keys);
	}

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(std, i, repeat);
		std::unordered_map<h64, int> map;	
		for (int key = 0, j = 0; key < keys; ++key, ++j)
			map[sequence[i][key]] = j * j + j;
	TEST_PERF_STOP();

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(core, i, repeat);
		DEFINE_ALLOCATOR();
		{
			DEFINE_HASHMAP(int, map);
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(sequence[i][key], j * j + i);
		}
		AllocatorDeinit(allocator);
	TEST_PERF_STOP();

	for (int i = 0; i < repeat; ++i) {
		delete[] sequence[i];
	}
	delete[] sequence;
}

TEST_PERF(HashMapHashAdd) {
	const unsigned long long keys = 10000;
	const unsigned long long repeat = 20;

	h64** sequence = new h64*[repeat];
	for (int i = 0; i < repeat; ++i) {
		sequence[i] = new h64[keys];
		for (int j = 0; j < keys; ++j) {
			sequence[i][j] = j;
		}
		std::random_shuffle(sequence[i], sequence[i] + keys);
	}

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(std, i, repeat);
		std::unordered_map<h64, int> map;
		for (int key = 0, j = 0; key < keys; ++key, ++j)
			map[sequence[i][key]] = j * j + j;
	TEST_PERF_STOP();

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(core, i, repeat);
		DEFINE_ALLOCATOR();
		{
			DEFINE_HASHMAP(int, map);
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::Hash(&sequence[i][key], sizeof(h64)), j * j + i);
		}
		AllocatorDeinit(allocator);
	TEST_PERF_STOP();


	for (h64 i = 0; i < repeat; ++i) {
		delete[] sequence[i];
	}
	delete[] sequence;
}

TEST_PERF(HashMapIterateValues) {
	const unsigned long long keys = 10000;
	const unsigned long long repeat = 20;

	h64** sequence = new h64*[repeat];
	for (int i = 0; i < repeat; ++i) {
		sequence[i] = new h64[keys];
		for (int j = 0; j < keys; ++j) {
			sequence[i][j] = j;
		}
		std::random_shuffle(sequence[i], sequence[i] + keys);
	}

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(std, i, repeat);
		std::unordered_map<h64, int> stdmap;
		for (int key = 0, j = 0; key < keys; ++key, ++j)
			stdmap[sequence[i][key]] = j + j;

	TEST_PERF_STOP();

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(core, i, repeat);
		DEFINE_ALLOCATOR();
		{
			DEFINE_HASHMAP(int, map);
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(sequence[i][key], j + j);
		}
		AllocatorDeinit(allocator);
	TEST_PERF_STOP();


	for (int i = 0; i < repeat; ++i) {
		delete[] sequence[i];
	}
	delete[] sequence;
}


TEST_PERF(HashMapNoHashGet) {
	const unsigned long long keys = 10000;
	const unsigned long long repeat = 20;

	h64** sequence = new h64*[repeat];
	for (int i = 0; i < repeat; ++i) {
		sequence[i] = new h64[keys];
		for (int j = 0; j < keys; ++j) {
			sequence[i][j] = j;
		}
		std::random_shuffle(sequence[i], sequence[i] + keys);
	}

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(std, i, repeat);
		std::unordered_map<h64, int> stdmap;
		for (int key = 0, j = 0; key < keys; ++key, ++j)
			stdmap[sequence[i][key]] = j + j;
	TEST_PERF_STOP();

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(core, i, repeat);
		DEFINE_ALLOCATOR();
		{
			DEFINE_HASHMAP(int, map);
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(sequence[i][key], j + j);
		}
		AllocatorDeinit(allocator);
	TEST_PERF_STOP();

	for (int i = 0; i < repeat; ++i) {
		delete[] sequence[i];
	}
	delete[] sequence;
}


TEST_PERF(HashMapHashGet) {
	const unsigned long long keys = 10000;
	const unsigned long long repeat = 20;

	h64** sequence = new h64*[repeat];
	for (int i = 0; i < repeat; ++i) {
		sequence[i] = new h64[keys];
		for (int j = 0; j < keys; ++j) {
			sequence[i][j] = j;
		}
		std::random_shuffle(sequence[i], sequence[i] + keys);
	}

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(std, i, repeat);
		std::unordered_map<h64, int> stdmap;
		for (int key = 0, j = 0; key < keys; ++key, ++j)
			stdmap[sequence[i][key]] = j + j;
	TEST_PERF_STOP();

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(core, i, repeat);
		DEFINE_ALLOCATOR();
		{
			DEFINE_HASHMAP(int, map);
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::Hash(&sequence[i][key], sizeof(h64)), j + j);
		}
		AllocatorDeinit(allocator);
	TEST_PERF_STOP();


	for (int i = 0; i < repeat; ++i) {
		delete[] sequence[i];
	}
	delete[] sequence;

}

TEST_PERF(HashMapFindRemove) {
	const unsigned long long keys = 10000;
	const unsigned long long repeat = 20;

	h64** sequence = new h64*[repeat];
	for (int i = 0; i < repeat; ++i) {
		sequence[i] = new h64[keys];
		for (int j = 0; j < keys; ++j) {
			sequence[i][j] = j;
		}
		std::random_shuffle(sequence[i], sequence[i] + keys);
	}
	
	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(std, i, repeat);
		std::unordered_map<h64, int> stdmap;
		for (int key = 0, j = 0; key < keys; ++key, ++j)
			stdmap[sequence[i][key]] = j + j;

		for (int key = 0; key < keys; ++key) {
			auto& it = stdmap.find(sequence[i][key]);
			if (it != stdmap.end()) {
				stdmap.erase(it);
			}
		}
	TEST_PERF_STOP();

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(core, i, repeat);
		DEFINE_ALLOCATOR();
		{
			DEFINE_HASHMAP(int, map);
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::Hash(&sequence[i][key], sizeof(h64)), j + j);

			for (int key = 0; key < keys; ++key) {
				auto hash = core::Hash(&sequence[i][key], sizeof(h64));
				auto valuep = map.Find(hash);
				if (valuep) {
					map.SwapRemove(hash);
				}
			}
		}
		AllocatorDeinit(allocator);
	TEST_PERF_STOP();


	for (int i = 0; i < repeat; ++i) {
		delete[] sequence[i];
	}
	delete[] sequence;
}

TEST_PERF(HashMapFindRemoveHalfInvalid) {
	const unsigned long long keys = 10000;
	const unsigned long long repeat = 20;

	h64** sequence = new h64*[repeat];
	for (int i = 0; i < repeat; ++i) {
		sequence[i] = new h64[keys];
		for (int j = 0; j < keys; ++j) {
			sequence[i][j] = j;
		}
		std::random_shuffle(sequence[i], sequence[i] + keys);
	}

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(std, i, repeat);
		std::unordered_map<h64, int> stdmap;
		for (int key = 0, j = 0; key < keys; ++key, ++j)
			stdmap[sequence[i][key]] = j + j;

		for (int key = 0; key < keys; ++key) {
			auto k = key % 2 == 0 ? sequence[i][key] : keys + sequence[i][key];
			auto& it = stdmap.find(k);
			if (it != stdmap.end()) {
				stdmap.erase(it);
			}
		}
	TEST_PERF_STOP();

	TestFlushCache2((void**) sequence, repeat, keys * sizeof(h64));

	TEST_PERF_START(core, i, repeat);
		DEFINE_ALLOCATOR();
		{
			DEFINE_HASHMAP(int, map);
			for (int key = 0, j = 0; key < keys; ++key, ++j)
				map.Add(core::Hash(&sequence[i][key], sizeof(h64)), j + j);

			int sum = 0;
			for (int key = 0; key < keys; ++key) {
				auto k = key % 2 == 0 ? sequence[i][key] : keys + sequence[i][key];
				auto hash = core::Hash(&k, sizeof(h64));
				auto valuep = map.Find(hash);
				if (valuep) {
					map.SwapRemove(hash);
				}
			}
		}
		AllocatorDeinit(allocator);
	TEST_PERF_STOP();


	for (int i = 0; i < repeat; ++i) {
		delete[] sequence[i];
	}
	delete[] sequence;
}

#undef DEFINE_ALLOCATOR
#undef DEFINE_HASHMAP
