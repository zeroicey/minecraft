#include <raylib.h>

void DrawUI()
{

    DrawText("Controls:", 10, 30, 20, DARKGRAY);
    DrawText("- W/A/S/D: Move", 10, 55, 20, DARKGRAY);
    DrawText("- Mouse: Look around", 10, 80, 20, DARKGRAY);
    DrawText("- Space: Fly up", 10, 105, 20, DARKGRAY);
    DrawText("- Left Shift: Fly down", 10, 130, 20, DARKGRAY);

    DrawFPS(10, 10);
}