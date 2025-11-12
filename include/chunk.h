#pragma once
#include "block.h"

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_DEPTH = 16;

struct ChunkCoord2D {
  int x, z;

  bool operator<(const ChunkCoord2D &other) const {
    if (x != other.x)
      return x < other.x;
    return z < other.z;
  }
};

class Chunk {
public:
  Chunk(const ChunkCoord2D &coord);
  BlockID getBlock(int localX, int localY, int localZ) const;
  void setBlock(int localX, int localY, int localZ, BlockID id);

private:
  BlockID m_blocks[CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH];
  ChunkCoord2D m_coord;

  bool m_isDirty = true;
};
