#pragma once
#include "config.h"
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
