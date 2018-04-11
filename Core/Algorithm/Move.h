#pragma once

#include <utility>

namespace core {
	using std::forward;
	using std::move;


/*	template<typename T>
	struct RemoveReference;

	template<typename T>
	struct RemoveReference { typedef T Type; };

	template<typename T>
	struct RemoveReference<T&> { typedef T Type; };

	template<typename T>
	struct RemoveReference<T&&> { typedef T Type; };
	
	template<typename T>
	typename RemoveReference<T>::Type&& move(T&& t) {
		return static_cast<typename RemoveReference<T>::Type&&>(t);
	}

	template<typename T>
	typename T&& forward(typename RemoveReference<T>::Type& t) {
		return static_cast<T&&>(t);
	}

	template<typename T>
	typename T&& forward(typename RemoveReference<T>::Type&& t) {
		return static_cast<T&&>(t);
	}*/


}