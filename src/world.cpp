#include "world.h"
#include "block.h"
#include "chunk.h"
#include "config.h"
#include "embedded_atlas.h"
#include "raylib.h"
#include "player.h"
#include <raymath.h>
#include <cmath>
#include <vector>
#include <algorithm>

TextureAtlas g_atlas;
Material g_blockMaterial;

void InitWorld()
{
  // 图集在编译期就已经被编进 exe 了（见 cmake/embed_binary.cmake），
  // 所以运行时不需要 assets/ 目录，发布只要发这一个 exe。
  if (!g_atlas.LoadFromMemory(EMBEDDED_ATLAS_PNG, (int)EMBEDDED_ATLAS_PNG_SIZE))
  {
    TraceLog(LOG_ERROR, "InitWorld: failed to load the embedded atlas");
  }

  // 所有方块共用一个材质：默认 shader + 图集贴图
  g_blockMaterial = LoadMaterialDefault();
  g_blockMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = g_atlas.GetTexture();
}

void GenerateChunkTerrain(Chunk *chunk, int chunkX, int chunkZ)
{
  for (int localX = 0; localX < CHUNK_WIDTH; localX++)
  {
    for (int localZ = 0; localZ < CHUNK_DEPTH; localZ++)
    {
      // 计算世界坐标
      int worldX = chunkX * CHUNK_WIDTH + localX;
      int worldZ = chunkZ * CHUNK_DEPTH + localZ;

      // 计算当前 (x, z) 坐标的地表高度
      float heightNoise =
          sinf(worldX / TERRAIN_FREQUENCY) * cosf(worldZ / TERRAIN_FREQUENCY);
      int surfaceHeight =
          (int)(TERRAIN_BASE_HEIGHT + heightNoise * TERRAIN_AMPLITUDE);

      // 确保高度在世界范围内
      if (surfaceHeight >= CHUNK_HEIGHT)
        surfaceHeight = CHUNK_HEIGHT - 1;
      if (surfaceHeight < 0)
        surfaceHeight = 0;

      // 从下往上填充方块
      for (int y = 0; y < CHUNK_HEIGHT; y++)
      {
        BlockType blockType;
        if (y > surfaceHeight)
        {
          blockType = BlockType::AIR;
        }
        else if (y == surfaceHeight)
        {
          blockType = BlockType::GRASS;
        }
        else if (y > surfaceHeight - STONE_LAYER_DEPTH)
        {
          blockType = BlockType::DIRT;
        }
        else
        {
          blockType = BlockType::STONE;
        }
        chunk->setBlock(localX, y, localZ, blockType);
      }
    }
  }
}

void UnloadWorldTextures()
{
  // 注意顺序：UnloadMaterial 会把材质 maps 里引用的贴图也一起卸载，
  // 而这张贴图归 g_atlas 管。先把引用摘掉，避免同一张贴图被卸两次。
  g_blockMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = Texture2D{0};
  UnloadMaterial(g_blockMaterial);
  g_atlas.Unload();
}

World::World()
{
  // 初始加载玩家周围的区块
  for (int x = -1; x <= 1; x++)
  {
    for (int z = -1; z <= 1; z++)
    {
      loadChunk(x, z);
    }
  }
}

World::~World()
{
  // 释放所有区块内存
  for (auto &pair : m_chunks)
  {
    delete pair.second;
  }
  m_chunks.clear();
}

int floor_div(int a, int n)
{
  int r = a / n;
  if ((a % n != 0) && ((a < 0) != (n < 0)))
  {
    r--;
  }
  return r;
}

BlockType World::getBlock(int worldX, int worldY, int worldZ) const
{
  if (worldY < 0 || worldY >= CHUNK_HEIGHT)
  {
    return BlockType::AIR;
  }

  ChunkCoord2D chunkCoord = worldToChunkCoord(worldX, worldZ);

  auto it = m_chunks.find(chunkCoord);
  if (it != m_chunks.end())
  {
    Chunk *chunk = it->second;

    int localX = worldX - chunkCoord.x * CHUNK_WIDTH;
    int localZ = worldZ - chunkCoord.z * CHUNK_DEPTH;

    return chunk->getBlock(localX, worldY, localZ);
  }

  return BlockType::AIR;
}

void World::setBlock(int worldX, int worldY, int worldZ, BlockType id)
{
  if (worldY < 0 || worldY >= CHUNK_HEIGHT)
  {
    return; // 不在世界高度范围内，直接忽略
  }
  ChunkCoord2D chunkCoord = worldToChunkCoord(worldX, worldZ);

  auto it = m_chunks.find(chunkCoord);
  if (it != m_chunks.end())
  {
    Chunk *chunk = it->second;
    int localX = worldX - chunkCoord.x * CHUNK_WIDTH;
    int localZ = worldZ - chunkCoord.z * CHUNK_DEPTH;

    chunk->setBlock(localX, worldY, localZ, id);
  }
}

void World::loadChunk(int x, int z)
{
  ChunkCoord2D coord = {x, z};
  // 已经加载过了就直接返回
  if (m_chunks.find(coord) != m_chunks.end())
  {
    return;
  }

  // 把 this 传进去：Chunk 需要它来查询跨区块的邻居方块
  Chunk *newChunk = new Chunk(coord, this);
  // 生成地形
  GenerateChunkTerrain(newChunk, x, z);
  m_chunks[coord] = newChunk;

  // 这个区块之前不存在，邻居生成 mesh 时把这里当成了空气（于是在接缝处
  // 留下了一堵朝外的"墙"）。现在它出现了，必须让已加载的 4 个邻居重建。
  const ChunkCoord2D neighbors[4] = {
      {x - 1, z}, {x + 1, z}, {x, z - 1}, {x, z + 1}};
  for (const ChunkCoord2D &n : neighbors)
  {
    auto it = m_chunks.find(n);
    if (it != m_chunks.end())
    {
      it->second->markDirty();
    }
  }
}

ChunkCoord2D World::worldToChunkCoord(int worldX, int worldZ) const
{
  int chunkX = floor_div(worldX, CHUNK_WIDTH);
  int chunkZ = floor_div(worldZ, CHUNK_DEPTH);
  return {chunkX, chunkZ};
}

void World::update(const Vector3 &playerPosition)
{
  // 定义加载和卸载的半径（以区块为单位）
  const int LOAD_RADIUS = 2;   // 加载玩家周围 5x5 个区块 (2*2+1)
  const int UNLOAD_RADIUS = 4; // 卸载距离玩家 4 个区块以外的区块

  // 计算玩家所在的区块坐标
  int playerChunkX = floor_div((int)playerPosition.x, CHUNK_WIDTH);
  int playerChunkZ = floor_div((int)playerPosition.z, CHUNK_DEPTH);

  // 1. 加载玩家周围的区块
  for (int offsetX = -LOAD_RADIUS; offsetX <= LOAD_RADIUS; offsetX++)
  {
    for (int offsetZ = -LOAD_RADIUS; offsetZ <= LOAD_RADIUS; offsetZ++)
    {
      int chunkX = playerChunkX + offsetX;
      int chunkZ = playerChunkZ + offsetZ;
      loadChunk(chunkX, chunkZ);
    }
  }

  // 2. 卸载距离玩家过远的区块
  std::vector<ChunkCoord2D> chunksToUnload;
  for (auto &pair : m_chunks)
  {
    ChunkCoord2D chunkCoord = pair.first;

    // 计算区块中心到玩家的距离（以区块为单位）
    int distanceX = abs(chunkCoord.x - playerChunkX);
    int distanceZ = abs(chunkCoord.z - playerChunkZ);
    int maxDistance = std::max(distanceX, distanceZ);

    // 如果超出卸载范围，标记为待卸载
    if (maxDistance > UNLOAD_RADIUS)
    {
      chunksToUnload.push_back(chunkCoord);
    }
  }

  // 执行卸载
  for (const auto &coord : chunksToUnload)
  {
    auto it = m_chunks.find(coord);
    if (it != m_chunks.end())
    {
      // 卸载前，把还活着的邻居标脏：它们之前贴着这个区块的面被剔掉了，
      // 现在要重新露出来。
      const ChunkCoord2D neighbors[4] = {
          {coord.x - 1, coord.z}, {coord.x + 1, coord.z}, {coord.x, coord.z - 1}, {coord.x, coord.z + 1}};
      for (const ChunkCoord2D &n : neighbors)
      {
        auto neighborIt = m_chunks.find(n);
        if (neighborIt != m_chunks.end())
        {
          neighborIt->second->markDirty();
        }
      }

      delete it->second;  // 释放区块内存
      m_chunks.erase(it); // 从map中移除
    }
  }

  // 3. 重建所有被标脏的区块的 mesh（干净的区块会在 update() 里直接跳过）
  for (auto &pair : m_chunks)
  {
    pair.second->update();
  }
}

void World::render()
{
  for (auto &pair : m_chunks)
  {
    ChunkCoord2D chunkCoord = pair.first;
    Chunk *chunk = pair.second;

    // 距离裁剪：区块中心到相机的水平距离超过阈值就跳过
    const int chunkCenterX = chunkCoord.x * CHUNK_WIDTH + CHUNK_WIDTH / 2;
    const int chunkCenterZ = chunkCoord.z * CHUNK_DEPTH + CHUNK_DEPTH / 2;
    const float dx = playerCamera.position.x - (float)chunkCenterX;
    const float dz = playerCamera.position.z - (float)chunkCenterZ;
    if (sqrtf(dx * dx + dz * dz) > 128.0f)
    {
      continue;
    }

    // mesh 在 Chunk::update() 里已经建好并上传到 GPU，这里只管画
    chunk->Draw();
  }
}
