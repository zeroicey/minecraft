#include "config.h"
#include "player.h"
#include "world.h"
#include "block.h"

#include <cstdio> // fprintf
#include <cstdlib> // getenv
#if defined(__linux__)
#include <dlfcn.h> // dlopen
#endif

void DrawMouse();
void DrawUI(const Camera3D &camera);

int main(void)
{
  // raylib 5.5 does not check InitPlatform()'s return value: when GLFW cannot
  // reach a display it only logs a warning, keeps going and then dies inside
  // rlglInit(), i.e. still inside InitWindow(). The last chance for a clean
  // message is therefore before that call.
#if defined(__linux__) || defined(__APPLE__)
  if (!getenv("DISPLAY") && !getenv("WAYLAND_DISPLAY"))
  {
    fprintf(stderr, "[main] no display: DISPLAY and WAYLAND_DISPLAY are both unset\n");
    return 1;
  }
#endif
#if defined(__linux__)
  // The same holds for the X11 client libraries: GLFW dlopen()s them by name
  // (Xcursor/Xrandr/Xi as well), so a NixOS-style profile path outside the
  // loader search path is enough to abort the process inside InitWindow().
  if (getenv("DISPLAY"))
  {
    void *x11 = dlopen("libX11.so.6", RTLD_LAZY);
    if (!x11)
    {
      fprintf(stderr, "[main] DISPLAY is set but libX11.so.6 cannot be loaded\n"
                      "        run via ./run.sh, or put the X11 libraries on the loader path\n");
      return 1;
    }
    dlclose(x11);
  }
#endif

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
