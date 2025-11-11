#include <raylib.h>

void DrawMouse() {
  float length = 10.0f;
  float thickness = 4.0f;
  Color color = BLACK;
  Vector2 center = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};

  // Draw vertical line
  DrawLineEx((Vector2){center.x, center.y - length},
             (Vector2){center.x, center.y + length}, thickness, BLACK);

  // Draw horizontal line
  DrawLineEx((Vector2){center.x - length, center.y},
             (Vector2){center.x + length, center.y}, thickness, BLACK);
}
