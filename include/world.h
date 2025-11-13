#pragma once
#include "chunk.h"
#include <map>
#include <raylib.h>

// 纹理资源（全局共享）
extern Texture2D grassTexture;
extern Texture2D stoneTexture;
extern Texture2D dirtTexture;

void InitWorld();
void UnloadWorldTextures();

class World {
public:
  World();
  ~World(); // 世界销毁时，需要释放所有区块的内存

  // 主要的更新函数，由游戏主循环调用
  void update(const Vector3 &playerPosition);

  // 获取世界中任意一个绝对坐标的方块ID
  BlockID getBlock(int worldX, int worldY, int worldZ) const;

  // 设置世界中任意一个绝对坐标的方块ID
  void setBlock(int worldX, int worldY, int worldZ, BlockID id);

  // 渲染所有加载的区块
  void render();

private:
  // 将世界坐标转换为区块坐标
  ChunkCoord2D worldToChunkCoord(int worldX, int worldZ) const;

  // 加载或生成一个指定的区块
  void loadChunk(int x, int z);

private:
  // 核心数据结构：存储所有当前加载的区块
  std::map<ChunkCoord2D, Chunk *> m_chunks;
};
