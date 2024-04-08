#pragma once

#include <assert.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#ifdef CYAPI
   #undef CYAPI
#endif
//#ifdef CYPHER_EXPORTS
//   #define CYAPI __declspec(dllexport)
//#else
//   #define CYAPI __declspec(dllimport)
//#endif
#ifdef _WIN32
   #define CYAPI __declspec(dllexport)
#else
   #define CYAPI __attribute__ ((visibility ("default")))
#endif

#define Assert(predicate) assert(predicate)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
