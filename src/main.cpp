#include "config.h"
#include "player.h"
#include "world.h"
#include "block.h"

void DrawMouse();
void DrawUI();

int main(void) {
  // 1. 初始化
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minecraft Clone");
  SetTargetFPS(TARGET_FPS);
  DisableCursor();

  BlockRegistry::Initialize();
  InitPlayer();
  InitWorld();
  
  // 创建世界对象（自动生成初始区块）
  World world;

  // 2. 游戏主循环
  while (!WindowShouldClose()) {
    // 2.1 更新数据
    UpdatePlayer(&world);
    world.update(playerCamera.position);

    // 2.2 绘制画面
    BeginDrawing();
    ClearBackground(SKYBLUE);

    BeginMode3D(playerCamera);
    world.render();
    EndMode3D();

    DrawUI();
    DrawMouse();
    EndDrawing();
  }

  // 3. 清理收尾
  UnloadWorldTextures();
  CloseWindow();

  return 0;
}
