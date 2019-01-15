#pragma once


#if defined(ENG_DLL_EXPORT)
#define ENG_EXPORT __declspec(dllexport)
#elif defined(ENG_DLL_IMPORT)
#define ENG_EXPORT __declspec(dllimport)
#else
#define ENG_EXPORT
#endif