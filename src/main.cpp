#include "config.h"
#include "player.h"
#include "world.h"
#include "block.h"

#include <cstdlib>

void DrawMouse();
void DrawUI(const Camera3D &camera);

int main(void)
{
  // 1. 初始化
  // 注意：raylib 5.5 的 InitWindow() 在 GLFW 初始化失败时**不会报错返回**，而是
  // 继续往下走到 rlglInit() -> rlLoadTexture()，通过空函数指针调用直接 SIGSEGV
  // （gdb: #0 0x0 <- rlLoadTexture <- rlglInit <- InitWindow <- main）。
  // 所以失败处理必须放在调用**之前**，调用之后再判 IsWindowReady() 只会是死代码。
#if defined(__linux__)
  if (std::getenv("DISPLAY") == nullptr && std::getenv("WAYLAND_DISPLAY") == nullptr)
  {
    TraceLog(LOG_ERROR, "no DISPLAY/WAYLAND_DISPLAY: run from a graphical session, not over plain ssh");
    return 1;
  }
#endif
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minecraft Clone");
  // glfwInit 成功、但创建窗口失败时 raylib 会正常返回，这时才轮到下面的判。
  if (!IsWindowReady())
  {
    TraceLog(LOG_ERROR, "InitWindow failed - see the raylib/GLFW log above");
    CloseWindow();
    return 1;
  }
  SetTargetFPS(TARGET_FPS);
  DisableCursor();

  BlockRegistry::Initialize();
  InitPlayer();
  InitWorld();

  // 创建世界对象（自动生成初始区块）
  World world;

  // 2. 游戏主循环
  while (!WindowShouldClose())
  {
    // 2.1 更新数据
    UpdatePlayer(&world);
    world.update(playerCamera.position);

    // 2.2 绘制画面
    BeginDrawing();
    ClearBackground(SKYBLUE);

    BeginMode3D(playerCamera);
    world.render();
    EndMode3D();

    DrawUI(playerCamera);
    DrawMouse();
    EndDrawing();
  }

  // 3. 清理收尾
  UnloadWorldTextures();
  CloseWindow();

  return 0;
}
