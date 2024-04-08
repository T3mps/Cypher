#pragma once

#include <algorithm>
#include <intrin.h>

#include "Common.h"
#include "Memory.h"

#ifdef CYPHER_DEBUG
   #define ALIGNED_MALLOC(size, alignment) _aligned_malloc_dbg(size, alignment, __FILE__, __LINE__)
   #define ALIGNED_FREE _aligned_free_dbg
#else
   #define ALIGNED_MALLOC(size, alignment) _aligned_malloc(size, alignment)
   #define ALIGNED_FREE _aligned_free
#endif

#define CYPHER_STACK_NEW(stackAllocator) \
    void* operator new(size_t size) { return stackAllocator.Allocate(static_cast<uint32_t>(size)); } \
    void operator delete(void* ptr) { if (ptr) stackAllocator.Deallocate(&ptr); }

#define CYPHER_STACK_NEW_WITH_ALIGNMENT(stackAllocator, alignment) \
    void* operator new(size_t size) { return stackAllocator.Allocate(static_cast<uint32_t>(size), alignment); } \
    void operator delete(void* ptr) { if (ptr) stackAllocator.Deallocate(&ptr); }

namespace Cypher
{
   template<uint32_t _stackSizeBytes, uint32_t _maxStackEntries, uint32_t _defaultAlignment, bool _prefetch>
   class StackAllocator;

   template<uint32_t _stackSizeBytes, uint32_t _maxStackEntries, uint32_t _defaultAlignment, bool _prefetch>
   class AllocationGuard
   {
   public:
      AllocationGuard(StackAllocator<_stackSizeBytes, _maxStackEntries, _defaultAlignment, _prefetch>& allocator, uint32_t size, bool usedMalloc) :
         m_allocator(allocator),
         m_size(size),
         m_usedMalloc(usedMalloc),
         m_committed(false)
      {}

      ~AllocationGuard()
      {
         if (!m_committed)
         {
            if (m_usedMalloc)
            {
               ALIGNED_FREE(m_allocator.m_entries[m_allocator.m_size].data);
            }
            else
            {
               m_allocator.m_index -= m_size;
            }
         }
      }

      void Commit()
      {
         m_committed = true;
         m_allocator.m_size++;
         m_allocator.m_allocation += m_size;
         m_allocator.m_maxAllocation = std::max<uint32_t>(m_allocator.m_maxAllocation, m_allocator.m_allocation);
      }

   private:
      StackAllocator<_stackSizeBytes, _maxStackEntries, _defaultAlignment, _prefetch>& m_allocator;
      uint32_t m_size;
      bool m_usedMalloc;
      bool m_committed;
   };

   struct StackEntry
   {
      char* data = nullptr;
      bool usedMalloc = false;
      uint32_t size = 0;
   };

   template<uint32_t _stackSizeBytes = 1000 * 1024, uint32_t _maxStackEntries = 128, uint32_t _defaultAlignment = 16, bool _prefetch = true>
   class StackAllocator
   {
   public:
      StackAllocator() :
         m_index(0),
         m_allocation(0),
         m_maxAllocation(0),
         m_mallocCount(0),
         m_size(0)
      {
#ifdef CYPHER_DEBUG
         std::memset(m_data, 0, _stackSizeBytes);
#endif
      }

      ~StackAllocator()
      {
         Assert(m_index == 0);
         Assert(m_size == 0);
      }

      void* Allocate(uint32_t size, uint32_t alignment = _defaultAlignment)
      {
         Assert(m_size < _maxStackEntries);

         size = (size + alignment - 1) & ~(alignment - 1);

         void* ptr = nullptr;
         bool usedMalloc = false;
         if (m_index + size <= _stackSizeBytes)
         {
            ptr = m_data + m_index;
            ptr = Align(ptr, alignment);
            uintptr_t newIndex = static_cast<uintptr_t>(reinterpret_cast<char*>(ptr) - m_data) + size;

            if (newIndex > _stackSizeBytes)
               throw std::bad_alloc();

            m_index = static_cast<uint32_t>(newIndex);
         }
         else
         {
            ptr = ALIGNED_MALLOC(size, alignment);
            if (!ptr)
               throw std::bad_alloc();
            ++m_mallocCount;
            usedMalloc = true;
         }

         AllocationGuard<_stackSizeBytes, _maxStackEntries, _defaultAlignment, _prefetch> guard(*this, size, usedMalloc);
         m_entries[m_size] = { static_cast<char*>(ptr), usedMalloc, size };

#ifdef CYPHER_DEBUG
         std::memset(ptr, 0, size);
#endif
         Assert((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0);

         guard.Commit();
         if (_prefetch && !usedMalloc)
         {
            char* nextPtr = m_data + m_index;
            if (m_index + size <= _stackSizeBytes)
               _mm_prefetch(nextPtr, _MM_HINT_T0);
         }
         return ptr;
      }

      void Deallocate(void** memory)
      {
         if (!memory || !*memory)
            return;

         Assert(m_size > 0);
         StackEntry* entry = m_entries + m_size - 1;
         Assert(*memory == entry->data);

         if (entry->usedMalloc)
            ALIGNED_FREE(*memory);
         else
            m_index -= entry->size;

         m_allocation -= entry->size;
         --m_size;

         *memory = nullptr;
      }

      uint32_t GetMaxAllocation() const { return m_maxAllocation; }
      uint32_t GetMallocCount() const { return m_mallocCount; }

   private:
      char m_data[_stackSizeBytes];
      uint32_t m_index;

      uint32_t m_allocation;
      uint32_t m_maxAllocation;
      uint32_t m_mallocCount;

      StackEntry m_entries[_maxStackEntries];
      uint32_t m_size;

      friend class AllocationGuard<_stackSizeBytes, _maxStackEntries, _defaultAlignment, _prefetch>;
   };
}
