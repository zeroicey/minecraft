#pragma once
// 玩家/相机设置
#define PLAYER_MOVE_SPEED 0.1f
#define PLAYER_MOUSE_SENSITIVITY 0.030f
#define PLAYER_INITIAL_POS                                                     \
  (Vector3){4.0f, 20.0f, 12.0f} // 初始位置高一点，防止卡在地面
#define PLAYER_INITIAL_TARGET (Vector3){4.0f, 20.0f, 11.0f}

// 窗口设置
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TARGET_FPS 120

// 地形生成参数
#define TERRAIN_BASE_HEIGHT 32.0f // 基础地平线
#define TERRAIN_AMPLITUDE 15.0f   // 地形起伏幅度
#define TERRAIN_FREQUENCY 10.0f   // 地形频率（数值越大，地形越平缓）
#define STONE_LAYER_DEPTH 5       // 从地表往下多少层开始变为石头
