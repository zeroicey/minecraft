#include "world.h"
#include "config.h"
#include "utils.h"
#include <raymath.h>

int world[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
Texture2D grassTexture;
Texture2D dirtTexture;
Texture2D stoneTexture;

void InitWorld()
{
  grassTexture = LoadTexturePNG("assets/grass.png");
  dirtTexture = LoadTexturePNG("assets/dirt.png");
  stoneTexture = LoadTexturePNG("assets/stone.png");
}

void GenerateWorld()
{
  for (int x = 0; x < CHUNK_WIDTH; x++)
  {
    for (int z = 0; z < CHUNK_LENGTH; z++)
    {
      // 1. 计算当前 (x, z) 坐标的地表高度
      float heightNoise =
          sinf(x / TERRAIN_FREQUENCY) * cosf(z / TERRAIN_FREQUENCY);
      int surfaceHeight =
          (int)(TERRAIN_BASE_HEIGHT + heightNoise * TERRAIN_AMPLITUDE);

      // 确保高度在世界范围内
      if (surfaceHeight >= CHUNK_HEIGHT)
        surfaceHeight = CHUNK_HEIGHT - 1;
      if (surfaceHeight < 0)
        surfaceHeight = 0;

      // 2. 从下往上填充方块
      for (int y = 0; y < CHUNK_HEIGHT; y++)
      {
        if (y > surfaceHeight)
        {
          // 在地表以上，全是空气
          world[x][y][z] = BLOCK_AIR;
        }
        else if (y == surfaceHeight)
        {
          // 地表最顶层，是草方块
          world[x][y][z] = BLOCK_GRASS;
        }
        else if (y > surfaceHeight - STONE_LAYER_DEPTH)
        {
          // 地表下面几层，是泥土
          world[x][y][z] = BLOCK_DIRT;
        }
        else
        {
          // 更深的地方，是石头
          world[x][y][z] = BLOCK_STONE;
        }
      }
    }
  }
}

int GetBlock(int x, int y, int z)
{
  if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 &&
      z < CHUNK_LENGTH)
  {
    // 2. 如果坐标在合法范围内，返回该位置的方块类型。
    return world[x][y][z];
  }
  else
  {
    return BLOCK_AIR;
  }
}

void RenderWorld()
{
  for (int x = 0; x < CHUNK_WIDTH; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_LENGTH; z++)
      {

        // 如果当前位置是空气，直接跳过，不进行任何渲染
        if (world[x][y][z] == BLOCK_AIR)
        {
          continue;
        }

        // 检查是否可见的逻辑 (这个优化依然非常重要!)
        bool isVisible = false;
        if (y + 1 >= CHUNK_HEIGHT || world[x][y + 1][z] == BLOCK_AIR)
          isVisible = true;
        else if (y - 1 < 0 || world[x][y - 1][z] == BLOCK_AIR)
          isVisible = true;
        else if (z + 1 >= CHUNK_LENGTH || world[x][y][z + 1] == BLOCK_AIR)
          isVisible = true;
        else if (z - 1 < 0 || world[x][y][z - 1] == BLOCK_AIR)
          isVisible = true;
        else if (x + 1 >= CHUNK_WIDTH || world[x + 1][y][z] == BLOCK_AIR)
          isVisible = true;
        else if (x - 1 < 0 || world[x - 1][y][z] == BLOCK_AIR)
          isVisible = true;

        if (isVisible)
        {
          Texture2D currentTexture;
          // 根据方块ID选择贴图
          switch (world[x][y][z])
          {
          case BLOCK_GRASS:
            currentTexture = grassTexture;
            break;
          case BLOCK_DIRT:
            currentTexture = dirtTexture;
            break;
          case BLOCK_STONE:
            currentTexture = stoneTexture;
            break;
          default:
            // 如果有未知的方块ID，可以给个默认或跳过
            continue;
          }
          DrawCubeTexture(currentTexture,
                          (Vector3){(float)x, (float)y, (float)z}, 1.0f, 1.0f,
                          1.0f, WHITE);
        }
      }
    }
  }
}

void DestroyBlockAt(int x, int y, int z) { world[x][y][z] = BLOCK_AIR; }

void PlaceBlockAt(int x, int y, int z, int block_type)
{
  world[x][y][z] = block_type;
}

void UnloadWorldTextures()
{
  UnloadTexture(grassTexture);
  UnloadTexture(stoneTexture);
  UnloadTexture(dirtTexture);
}
