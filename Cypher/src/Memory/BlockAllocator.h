#pragma once

#include <algorithm>

#include "Common.h"

namespace Cypher
{
   static constexpr uint32_t BLOCK_COUNT     = 14;
   static constexpr uint32_t MAX_BLOCK_SIZE  = 640;
   static constexpr uint32_t CHUNK_SIZE      = 16 * 1024;
   static constexpr uint32_t CHUNK_STRIDE    = 128;

   static constexpr uint32_t BLOCK_SIZES[BLOCK_COUNT] =
   {
      16,   // 0
      32,   // 1
      64,   // 2
      96,   // 3
      128,	// 4
      160,	// 5
      192,  // 6
      224,	// 7
      256,	// 8
      320,	// 9
      384,	// 10
      448,	// 11
      512,	// 12
      640,	// 13
   };

   struct SizeMap
   {
      SizeMap()
      {
         size_t j = 0;
         values[0] = 0;
         for (size_t i = 1; i <= MAX_BLOCK_SIZE; ++i)
         {
            Assert(j < BLOCK_COUNT);
            if (i <= BLOCK_SIZES[j])
            {
               values[i] = static_cast<uint8_t>(j);
            }
            else
            {
               ++j;
               values[i] = static_cast<uint8_t>(j);
            }
         }
      }

      uint8_t values[MAX_BLOCK_SIZE + 1];
   };

   struct Block
   {
      Block* next;
   };

   struct Chunk
   {
      uint32_t blockSize;
      Block* blocks;
   };

   class CYAPI BlockAllocator
   {
   public:
      BlockAllocator();
      ~BlockAllocator();

      void* Allocate(uint32_t size);
      void Deallocate(void* memory, uint32_t size);

      void Reset();

   private:
      static constexpr size_t SizeToIndex(uint32_t size) { return std::upper_bound(BLOCK_SIZES, BLOCK_SIZES + BLOCK_COUNT, size) - BLOCK_SIZES; }

      Chunk* m_chunks;
      uint32_t m_chunkCount;
      uint32_t m_chunkSpace;

      Block* m_freeLists[BLOCK_COUNT];
   };
} // namespace Cypher
