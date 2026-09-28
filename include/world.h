#pragma once
#include "chunk.h"
#include "texture_atlas.h"
#include <map>
#include <raylib.h>

// 纹理资源（全局共享）：整张图集只加载一次，具体贴哪一格由 UV 决定
extern TextureAtlas g_atlas;

// 所有方块共用的一份材质：raylib 默认 shader + 图集贴图
extern Material g_blockMaterial;

void InitWorld();
void UnloadWorldTextures();

class World {
public:
  World();
  ~World(); // 世界销毁时，需要释放所有区块的内存

  // 主要的更新函数，由游戏主循环调用
  void update(const Vector3 &playerPosition);

  // 获取世界中任意一个绝对坐标的方块Type
  BlockType getBlock(int worldX, int worldY, int worldZ) const;

  // 设置世界中任意一个绝对坐标的方块Type
  void setBlock(int worldX, int worldY, int worldZ, BlockType type);

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
