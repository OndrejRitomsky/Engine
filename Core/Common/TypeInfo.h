#pragma once

#include "Core/Common/Types.h"

namespace core {
	
/*	template<typename Fst>
	void TypesInfo(u32& size, u32& alignment);

	template<typename Fst, typename Snd, typename... Rest>
	void TypesInfo(u32& size, u32& alignment);


	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	template<typename Fst>
	inline void TypesInfo(u32& size, u32& alignment) {
		size = sizeof(Fst);
		alignment = alignof(Fst);
	}


	//-------------------------------------------------------------------------
	template<typename Fst, typename Snd, typename... Rest>
	inline void TypesInfo(u32& size, u32& alignment) {
		TypesInfo<Snd, Rest...>(size, alignment);

		size += sizeof(Fst);
		alignment += alignof(Fst);
	}*/
}