#include "config.h"
#include "player.h"
#include "world.h"
#include "block.h"

#include <cstdio> // fprintf
#include <cstdlib> // std::getenv
#if defined(__linux__)
#include <dlfcn.h> // dlopen / RTLD_LAZY
#endif

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
#if defined(__linux__)
  // 同一个道理适用于 X11 客户端库：GLFW 是按名字 dlopen() 它们的（Xcursor/Xrandr/Xi…），
  // 在 NixOS 这类没有 ldconfig 缓存的系统上，只要 profile 路径不在 loader 搜索路径里，
  // 进程就会在 InitWindow() 内部死掉。链接期补 -Wl,-rpath 是根治，这里是提前一句人话。
  if (std::getenv("DISPLAY") != nullptr)
  {
    void *x11 = dlopen("libX11.so.6", RTLD_LAZY);
    if (x11 == nullptr)
    {
      TraceLog(LOG_ERROR, "DISPLAY is set but libX11.so.6 cannot be loaded - use ./run.sh, or put the X11 libraries on the loader path");
      return 1;
    }
    dlclose(x11);
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
