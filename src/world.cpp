#include "world.h"
#include "block.h"
#include "chunk.h"
#include "config.h"
#include "raylib.h"
#include "utils.h"
#include <raymath.h>
#include <cmath>

Texture2D grassTexture;
Texture2D dirtTexture;
Texture2D stoneTexture;

void InitWorld() {
  grassTexture = LoadTexturePNG("assets/grass.png");
  dirtTexture = LoadTexturePNG("assets/dirt.png");
  stoneTexture = LoadTexturePNG("assets/stone.png");
}

void GenerateChunkTerrain(Chunk* chunk, int chunkX, int chunkZ) {
  for (int localX = 0; localX < CHUNK_WIDTH; localX++) {
    for (int localZ = 0; localZ < CHUNK_DEPTH; localZ++) {
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
      for (int y = 0; y < CHUNK_HEIGHT; y++) {
        BlockID blockType;
        if (y > surfaceHeight) {
          blockType = BlockID::AIR;
        } else if (y == surfaceHeight) {
          blockType = BlockID::GRASS;
        } else if (y > surfaceHeight - STONE_LAYER_DEPTH) {
          blockType = BlockID::DIRT;
        } else {
          blockType = BlockID::STONE;
        }
        chunk->setBlock(localX, y, localZ, blockType);
      }
    }
  }
}







void UnloadWorldTextures() {
  UnloadTexture(grassTexture);
  UnloadTexture(stoneTexture);
  UnloadTexture(dirtTexture);
}

World::World() {
  // 初始加载玩家周围的区块
  for (int x = -1; x <= 1; x++) {
    for (int z = -1; z <= 1; z++) {
      loadChunk(x, z);
    }
  }
}

World::~World() {
  // 释放所有区块内存
  for (auto &pair : m_chunks) {
    delete pair.second;
  }
  m_chunks.clear();
}

int floor_div(int a, int n) {
  int r = a / n;
  if ((a % n != 0) && ((a < 0) != (n < 0))) {
    r--;
  }
  return r;
}

BlockID World::getBlock(int worldX, int worldY, int worldZ) const {
  if (worldY < 0 || worldY >= CHUNK_HEIGHT) {
    return BlockID::AIR;
  }

  ChunkCoord2D chunkCoord = worldToChunkCoord(worldX, worldZ);

  auto it = m_chunks.find(chunkCoord);
  if (it != m_chunks.end()) {
    Chunk *chunk = it->second;

    int localX = worldX - chunkCoord.x * CHUNK_WIDTH;
    int localZ = worldZ - chunkCoord.z * CHUNK_DEPTH;

    return chunk->getBlock(localX, worldY, localZ);
  }

  return BlockID::AIR;
}

void World::setBlock(int worldX, int worldY, int worldZ, BlockID id) {
  if (worldY < 0 || worldY >= CHUNK_HEIGHT) {
    return; // 不在世界高度范围内，直接忽略
  }
  ChunkCoord2D chunkCoord = worldToChunkCoord(worldX, worldZ);

  auto it = m_chunks.find(chunkCoord);
  if (it != m_chunks.end()) {
    Chunk *chunk = it->second;
    int localX = worldX - chunkCoord.x * CHUNK_WIDTH;
    int localZ = worldZ - chunkCoord.z * CHUNK_DEPTH;

    chunk->setBlock(localX, worldY, localZ, id);
  }
}

void World::loadChunk(int x, int z) {
  ChunkCoord2D coord = {x, z};
  // 检查是否已经加载过了
  if (m_chunks.find(coord) == m_chunks.end()) {
    // 如果没有，就创建一个新的区块并放入map
    Chunk* newChunk = new Chunk(coord);
    // 生成地形
    GenerateChunkTerrain(newChunk, x, z);
    m_chunks[coord] = newChunk;
  }
}

ChunkCoord2D World::worldToChunkCoord(int worldX, int worldZ) const {
  int chunkX = floor_div(worldX, CHUNK_WIDTH);
  int chunkZ = floor_div(worldZ, CHUNK_DEPTH);
  return {chunkX, chunkZ};
}

void World::update(const Vector3 &playerPosition) {
  // TODO: 动态加载/卸载区块
}

void World::render() {
  for (auto &pair : m_chunks) {
    ChunkCoord2D chunkCoord = pair.first;
    Chunk *chunk = pair.second;

    // 遍历区块内的所有方块
    for (int localX = 0; localX < CHUNK_WIDTH; localX++) {
      for (int localY = 0; localY < CHUNK_HEIGHT; localY++) {
        for (int localZ = 0; localZ < CHUNK_DEPTH; localZ++) {
          BlockID blockID = chunk->getBlock(localX, localY, localZ);

          // 跳过空气方块
          if (blockID == BlockID::AIR) {
            continue;
          }

          // 计算世界坐标
          int worldX = chunkCoord.x * CHUNK_WIDTH + localX;
          int worldY = localY;
          int worldZ = chunkCoord.z * CHUNK_DEPTH + localZ;

          // 检查是否可见（至少有一个相邻方块是空气）
          bool isVisible = false;
          if (getBlock(worldX, worldY + 1, worldZ) == BlockID::AIR)
            isVisible = true;
          else if (getBlock(worldX, worldY - 1, worldZ) == BlockID::AIR)
            isVisible = true;
          else if (getBlock(worldX + 1, worldY, worldZ) == BlockID::AIR)
            isVisible = true;
          else if (getBlock(worldX - 1, worldY, worldZ) == BlockID::AIR)
            isVisible = true;
          else if (getBlock(worldX, worldY, worldZ + 1) == BlockID::AIR)
            isVisible = true;
          else if (getBlock(worldX, worldY, worldZ - 1) == BlockID::AIR)
            isVisible = true;

          if (isVisible) {
            Texture2D currentTexture;
            switch (blockID) {
            case BlockID::GRASS:
              currentTexture = grassTexture;
              break;
            case BlockID::DIRT:
              currentTexture = dirtTexture;
              break;
            case BlockID::STONE:
              currentTexture = stoneTexture;
              break;
            default:
              continue;
            }
            DrawCubeTexture(currentTexture,
                            (Vector3){(float)worldX, (float)worldY,
                                      (float)worldZ},
                            1.0f, 1.0f, 1.0f, WHITE);
          }
        }
      }
    }
  }
}
