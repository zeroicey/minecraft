#include <raylib.h>

void DrawUI(const Camera3D &camera)
{
    DrawFPS(10, 10);
    DrawText(TextFormat("X: %.3f Y: %.5f Z: %.3f",
                        camera.position.x, camera.position.y, camera.position.z),
             10, 30, 20, DARKGRAY);
}
