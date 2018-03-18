#pragma once

#include <utility>

// @TODO own implementation

namespace core {
	using std::forward;
	using std::move;

	template<typename T> 
	T Min(const T& a, const T& b);

	template<typename T> 
	T Max(const T& a, const T& b);

	template<typename T> 
	T Clamp(T value, T min, T max);

	template<typename T> 
	void Clamp(T& value, const T min, const T max);

	template<typename T> 
	void Swap(T& a, T& b);



	template<typename>
	struct removepointer;

	template<typename T>
	struct removepointer<T*> {
		typedef T Type;
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------


	//----------------------------------------------------------------------------
	template<typename T> 
	inline T Min(const T& a, const T& b) {
		return b > a ? a : b;
	}

	//----------------------------------------------------------------------------
	template<typename T> 
	inline T Max(const T& a, const T& b) {
		return b > a ? b : a;
	}

	//----------------------------------------------------------------------------
	template<typename T> 
	inline T Clamp(T value, T min, T max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	//----------------------------------------------------------------------------
	template<typename T> 
	inline void Clamp(T& value, T min, T max) {
		if (value < min) value = min;
		if (value > max) value = max;
	}

	//----------------------------------------------------------------------------
	template<typename T> 
	inline bool IsPowerOfTwo(T x) {
		return x != 0 && !(x & (x - 1));
	}

	//----------------------------------------------------------------------------
	template<typename T> 
	inline void Swap(T& a, T& b) {
		T c = a;
		a = b;
		b = c;
	}
}