#pragma once

#ifdef CYAPI
   #undef CYAPI
#endif
#ifdef CYPHER_EXPORTS
   #define CYAPI __declspec(dllexport)
#else
   #define CYAPI __declspec(dllimport)
#endif

namespace Cypher {}