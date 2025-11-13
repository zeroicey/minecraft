#include "player.h"
#include "block.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include <cmath>
#include <iostream>

// 定义玩家相机
Camera3D playerCamera = {0};

void InitPlayer() {
  playerCamera.position = PLAYER_INITIAL_POS;
  playerCamera.target = PLAYER_INITIAL_TARGET;
  playerCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
  playerCamera.fovy = 60.0f;
  playerCamera.projection = CAMERA_PERSPECTIVE;
}

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

void UpdatePlayer() {
  if (IsKeyPressed(KEY_F11)) {
    ToggleFullscreen();
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    Vector3 direction = Vector3Normalize(
        Vector3Subtract(playerCamera.target, playerCamera.position));
    Vector3 start_postition = playerCamera.position;

    Vector3 target_block_pos;

    for (float step = 0.1f; step < 8.0f; step += 0.1f) {
      Vector3 current_point =
          Vector3Add(start_postition, Vector3Scale(direction, step));
      int block_x = (int)round(current_point.x);
      int block_y = (int)round(current_point.y);
      int block_z = (int)round(current_point.z);
      if (GetBlock(block_x, block_y, block_z) != BlockID::AIR) {
        target_block_pos =
            (Vector3){(float)block_x, (float)block_y, (float)block_z};
        DestroyBlockAt(block_x, block_y, block_z);
        break;
      }
    }
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    Vector3 direction = Vector3Normalize(
        Vector3Subtract(playerCamera.target, playerCamera.position));
    Vector3 start_postition = playerCamera.position;

    Vector3 target_block_pos;
    Vector3 last_air_pos = start_postition;

    for (float step = 0.1f; step < 8.0f; step += 0.1f) {
      Vector3 current_point =
          Vector3Add(start_postition, Vector3Scale(direction, step));
      int block_x = (int)round(current_point.x);
      int block_y = (int)round(current_point.y);
      int block_z = (int)round(current_point.z);
      if (GetBlock(block_x, block_y, block_z) != BlockID::AIR) {
        PlaceBlockAt((int)round(last_air_pos.x), (int)round(last_air_pos.y),
                     (int)round(last_air_pos.z), BlockID::STONE);
        break;
      } else {
        last_air_pos =
            (Vector3){(float)block_x, (float)block_y, (float)block_z};
      }
    }
  }

  UpdateCameraPro(
      &playerCamera,
      (Vector3){(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)) * PLAYER_MOVE_SPEED,
                (IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * PLAYER_MOVE_SPEED,
                (IsKeyDown(KEY_SPACE) - IsKeyDown(KEY_LEFT_SHIFT)) *
                    PLAYER_MOVE_SPEED},
      (Vector3){GetMouseDelta().x * PLAYER_MOUSE_SENSITIVITY,
                GetMouseDelta().y * PLAYER_MOUSE_SENSITIVITY, 0.0f},
      -GetMouseWheelMove() * 2.0f);
}
