#include "BlockAllocator.h"

#include <cstddef>
#include <limits>

Cypher::BlockAllocator::BlockAllocator()
{
	assert(BLOCK_COUNT < std::numeric_limits<uint8_t>::max());

	m_chunks = new Chunk[CHUNK_STRIDE]{};
	m_chunkSpace = CHUNK_STRIDE;
	m_chunkCount = 0;
	std::fill(m_freeLists, m_freeLists + BLOCK_COUNT, nullptr);
}

Cypher::BlockAllocator::~BlockAllocator()
{
	auto freeChunk = [this](Chunk& chunk) { delete[] chunk.blocks; };
	std::for_each(m_chunks, m_chunks + m_chunkCount, freeChunk);
	delete[] m_chunks;
}

void* Cypher::BlockAllocator::Allocate(uint32_t size)
{
	if (size == 0 || size > MAX_BLOCK_SIZE)
		return nullptr;

	size_t index = SizeToIndex(size);
	assert(index < BLOCK_COUNT);

	if (m_freeLists[index]) {
		Block* block = m_freeLists[index];
		m_freeLists[index] = block->next;
		return block;
	}

	if (m_chunkCount == m_chunkSpace)
	{
		Chunk* newChunks = new Chunk[static_cast<size_t>(m_chunkSpace) + CHUNK_STRIDE]{};
		std::copy(m_chunks, m_chunks + m_chunkCount, newChunks);
		delete[] m_chunks;
		m_chunks = newChunks;
		m_chunkSpace += CHUNK_STRIDE;
	}

	Assert(index < BLOCK_COUNT);

	uint32_t blockSize = BLOCK_SIZES[index];

	Chunk& newChunk = m_chunks[m_chunkCount++];
	newChunk.blocks = new Block[CHUNK_SIZE / blockSize];

	uint8_t* blockBytes = reinterpret_cast<uint8_t*>(newChunk.blocks);
	for (uint32_t i = 0; i < CHUNK_SIZE / blockSize - 1; ++i)
	{
		Block* currentBlock = reinterpret_cast<Block*>(blockBytes + static_cast<size_t>(blockSize) * i);
		Block* nextBlock = reinterpret_cast<Block*>(blockBytes + static_cast<size_t>(blockSize) * (static_cast<size_t>(i) + 1));
		currentBlock->next = nextBlock;
	}

	Block* lastBlock = reinterpret_cast<Block*>(blockBytes + static_cast<size_t>(blockSize) * (CHUNK_SIZE / blockSize - 1));
	lastBlock->next = nullptr;

	newChunk.blockSize = blockSize;

	Block* allocatedBlock = newChunk.blocks;
	m_freeLists[index] = allocatedBlock->next;

	return allocatedBlock;
}

void Cypher::BlockAllocator::Deallocate(void* memory, uint32_t size)
{
	if (memory == nullptr || size == 0 || size > MAX_BLOCK_SIZE)
		return;

	size_t index = SizeToIndex(size);
	assert(index < BLOCK_COUNT);

	Block* block = static_cast<Block*>(memory);
	block->next = m_freeLists[index];
	m_freeLists[index] = block;
}

void Cypher::BlockAllocator::Reset()
{
	for (uint32_t i = 0; i < m_chunkCount; ++i)
	{
		delete[] m_chunks[i].blocks;
	}

	m_chunkCount = 0;
	std::fill(m_chunks, m_chunks + m_chunkSpace, Chunk{});
}
