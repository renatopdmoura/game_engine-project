#pragma once
#ifdef OUTPUTLIBRARY_EXPORTS
#define OUTPUTLIBRARY_API __declspec(dllexport)
#else
#define OUTPUTLIBRARY_API __declspec(dllimport)
#endif

extern "C" OUTPUTLIBRARY_API void simple_output(void);