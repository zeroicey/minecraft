#pragma once
#include <raylib.h>

// 声明所有 utils.cpp 中的函数
void DrawCubeTexture(Texture2D texture, Vector3 position, float width,
                     float height, float length, Color color);
enum CubeFaceMask : unsigned int
{
    FACE_FRONT = 1u << 0,
    FACE_BACK = 1u << 1,
    FACE_TOP = 1u << 2,
    FACE_BOTTOM = 1u << 3,
    FACE_RIGHT = 1u << 4,
    FACE_LEFT = 1u << 5,
    FACE_ALL = FACE_FRONT | FACE_BACK | FACE_TOP | FACE_BOTTOM | FACE_RIGHT | FACE_LEFT
};

// 新增：只绘制指定外露面的版本
void DrawCubeTextureMasked(Texture2D texture, Vector3 position, float width,
                           float height, float length, Color color,
                           unsigned int faceMask);
Texture2D LoadTexturePNG(const char *fileName);
