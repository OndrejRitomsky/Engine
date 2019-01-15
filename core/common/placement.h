#pragma once


namespace _internal {
	struct Placeholder {
	};
}


inline void* operator new (size_t, _internal::Placeholder, void* mem) {
	return mem;
}

inline void operator delete (void*, _internal::Placeholder, void*) {
}

#define Placement(mem) new ({}, (mem))



