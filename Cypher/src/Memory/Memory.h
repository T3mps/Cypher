#pragma once

#include "Common.h"

#define CYPHER_RESTORE_NEW() \
    void* operator new(size_t size) { return ::operator new(size); } \
    void operator delete(void* ptr) { ::operator delete(ptr); }

#define CYPHER_RESTORE_NEW_WITH_ALIGNMENT(alignment) \
    void* operator new(size_t size) { return ::operator new(size, std::align_val_t(alignment)); } \
    void operator delete(void* ptr) { ::operator delete(ptr); }

namespace Cypher
{
   static void* Align(void* ptr, uint32_t alignment)
   {
      uintptr_t rawAddress = reinterpret_cast<uintptr_t>(ptr);
      uintptr_t misalignment = rawAddress & (alignment - 1);
      if (misalignment == 0)
         return ptr;

      uintptr_t adjustment = alignment - misalignment;
      uintptr_t alignedAddress = rawAddress + adjustment;
      return reinterpret_cast<void*>(alignedAddress);
   }
}