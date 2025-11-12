#pragma once
#include "chunk.h"
#include "config.h"
#include <map>
#include <raylib.h>

// 声明世界数据结构和操作函数
extern int world[CHUNK_WIDTH][CHUNK_HEIGHT]
                [CHUNK_LENGTH]; // 使用 extern 声明全局变量
extern Texture2D grassTexture;
extern Texture2D stoneTexture;
extern Texture2D dirtTexture;

void InitWorld();
void GenerateWorld();
void RenderWorld();
void UnloadWorldTextures();
int GetBlock(int x, int y, int z);
void DestroyBlockAt(int x, int y, int z);
void PlaceBlockAt(int x, int y, int z, int block_type);

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

  // TODO: 渲染所有加载的区块
  // void render(const /* 摄像机 */& camera);

private:
  // 将世界坐标转换为区块坐标
  ChunkCoord2D worldToChunkCoord(int worldX, int worldZ) const;

  // 加载或生成一个指定的区块
  void loadChunk(int x, int z);

private:
  // 核心数据结构：存储所有当前加载的区块
  std::map<ChunkCoord2D, Chunk *> m_chunks;
};
