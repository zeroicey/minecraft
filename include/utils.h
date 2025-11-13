#pragma once
#include <raylib.h>

// 声明所有 utils.cpp 中的函数
void DrawCubeTexture(Texture2D texture, Vector3 position, float width,
                     float height, float length, Color color);
Texture2D LoadTexturePNG(const char *fileName);