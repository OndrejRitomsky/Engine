#pragma once

#include "Core/Types.h"

namespace core {
	// Sums sizeof, Sums alignof, Calculates max alignof for all types
	template<typename Fst>
	void TypesInfo(u32& size, u32& alignment, u32& maxAlignment);

	template<typename Fst, typename Snd, typename... Rest>
	void TypesInfo(u32& size, u32& alignment, u32& maxAlignment);


	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	template<typename Fst>
	inline void TypesInfo(u32& size, u32& alignment, u32& maxAllignment) {
		size = sizeof(Fst);
		alignment = alignof(Fst);
		maxAllignment = alignof(Fst);
	}


	//-------------------------------------------------------------------------
	template<typename Fst, typename Snd, typename... Rest>
	inline void TypesInfo(u32& size, u32& alignment, u32& maxAllignment) {
		TypesInfo<Snd, Rest...>(size, alignment, maxAllignment);

		size += sizeof(Fst);
		alignment += alignof(Fst);

		if (alignof(Fst) > maxAllignment)
			maxAllignment = alignof(Fst);
	}
}