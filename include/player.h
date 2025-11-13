#pragma once
#include <raylib.h>

class World;  // 前向声明

extern Camera3D playerCamera; // 声明玩家相机

void InitPlayer();
void UpdatePlayer(World* world);
