#include "chunk.h"
#include "block.h"
#include <algorithm>
#include <cassert>
#include <iterator>

Chunk::Chunk(const ChunkCoord2D &coord) : m_coord(coord), m_isDirty(true) {
  std::fill(std::begin(m_blocks), std::end(m_blocks), BlockID::AIR);
}

BlockID Chunk::getBlock(int localX, int localY, int localZ) const {
  assert(localX >= 0 && localX < CHUNK_WIDTH);
  assert(localY >= 0 && localY < CHUNK_HEIGHT);
  assert(localZ >= 0 && localZ < CHUNK_DEPTH);

  const int index =
      localY * (CHUNK_WIDTH * CHUNK_DEPTH) + localZ * CHUNK_WIDTH + localX;

  return m_blocks[index];
}

void Chunk::setBlock(int localX, int localY, int localZ, BlockID id) {
  assert(localX >= 0 && localX < CHUNK_WIDTH);
  assert(localY >= 0 && localY < CHUNK_HEIGHT);
  assert(localZ >= 0 && localZ < CHUNK_DEPTH);

  const int index =
      localY * (CHUNK_WIDTH * CHUNK_DEPTH) + localZ * CHUNK_WIDTH + localX;

  if (m_blocks[index] != id) {
    m_blocks[index] = id;
    m_isDirty = true;
  }
}
