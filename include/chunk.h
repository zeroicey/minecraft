#pragma once
#include "block.h"
#include "raylib.h"
#include "texture_atlas.h"
#include <vector>

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_DEPTH = 16;

// 前向声明：Chunk 只需要一个 World 指针，不需要 World 的完整定义。
// 这里不能 #include "world.h" —— world.h 里又 include 了 chunk.h，会循环包含。
class World;

// 面的枚举标识，方便查找
enum FaceDirection
{
  FACE_TOP = 0,
  FACE_BOTTOM,
  FACE_RIGHT,
  FACE_LEFT,
  FACE_FRONT,
  FACE_BACK
};

// 辅助函数命名建议：AddFaceData
void AddFaceData(
    std::vector<float> &vertices,
    std::vector<float> &normals,
    std::vector<float> &texcoords,
    int faceIndex,
    float x, float y, float z,
    BlockType blockType);

// 查 (方块类型, 面朝向) 在图集里对应哪一格，返回它的 UV 矩形
UVRect GetBlockUV(BlockType blockType, int faceIndex);

struct ChunkCoord2D
{
  int x, z;

  bool operator<(const ChunkCoord2D &other) const
  {
    if (x != other.x)
      return x < other.x;
    return z < other.z;
  }
};

class Chunk
{
public:
  Chunk(const ChunkCoord2D &coord, const World *world);
  ~Chunk();
  BlockType getBlock(int localX, int localY, int localZ) const;
  void setBlock(int localX, int localY, int localZ, BlockType type);

  void update();
  void Draw();

  // 标记"需要重建 mesh"。邻居区块加载后，边界上的面会变化，必须重算。
  void markDirty() { m_isDirty = true; }

private:
  BlockType m_blocks[CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH];
  ChunkCoord2D m_coord;

  // 反向指针，用于跨区块查询邻居方块。
  // 不拥有 World，生命周期由 World 保证（World 析构时会先 delete 所有 Chunk）。
  const World *m_world;

  Mesh m_mesh;

  bool m_isDirty;
  bool m_hasMesh;

  void buildMesh();
};
