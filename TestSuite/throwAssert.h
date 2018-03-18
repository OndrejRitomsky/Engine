#pragma once


#ifndef ASSERT 
#error ASSERT NOT DEFINED, include assert first or rename it to ASSERT
#endif

#ifndef ASSERT2 
#error ASSERT2 NOT DEFINED, include assert2 first or rename it to ASSERT2
#endif

#undef ASSERT
#undef ASSERT2

#define ASSERT(value) {if (!(value)) throw("ASSERT TEST");}
#define ASSERT2(value, message) {if (!(value)) throw("ASSERT TEST");}
