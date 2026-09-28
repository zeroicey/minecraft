#include "chunk.h"
#include "block.h"
#include "world.h"
#include "raylib.h"
#include "utils.h"
#include <algorithm>
#include <raymath.h>
#include <cassert>
#include <iterator>
#include <vector>

struct Direction
{
  int dx, dy, dz;
  Vector3 normal;
};

// 预定义 6 个方向偏移及其法线 (Y+, Y-, X+, X-, Z+, Z-)
static constexpr Direction DIRECTIONS[6] = {
    {0, 1, 0, {0.0f, 1.0f, 0.0f}},   // Top
    {0, -1, 0, {0.0f, -1.0f, 0.0f}}, // Bottom
    {1, 0, 0, {1.0f, 0.0f, 0.0f}},   // Right
    {-1, 0, 0, {-1.0f, 0.0f, 0.0f}}, // Left
    {0, 0, 1, {0.0f, 0.0f, 1.0f}},   // Front
    {0, 0, -1, {0.0f, 0.0f, -1.0f}}  // Back
};

Chunk::Chunk(const ChunkCoord2D &coord, const World *world)
    : m_coord(coord), m_world(world), m_mesh{}, m_isDirty(true), m_hasMesh(false)
{
  std::fill(std::begin(m_blocks), std::end(m_blocks), BlockType::AIR);
}

Chunk::~Chunk()
{
  if (this->m_hasMesh)
  {
    UnloadMesh(this->m_mesh);
  }
}

void AddFaceData(
    std::vector<float> &vertices,
    std::vector<float> &normals,
    std::vector<float> &texcoords,
    int faceIndex,
    float x, float y, float z,
    BlockType blockType)
{
  // 1. 写入法线（6 个顶点共享同一条法线）
  Vector3 n = DIRECTIONS[faceIndex].normal;
  for (int i = 0; i < 6; i++)
  {
    normals.push_back(n.x);
    normals.push_back(n.y);
    normals.push_back(n.z);
  }

  // 2. 根据面朝向，写入 6 个顶点的坐标 (2 个三角形)
  // 注意：每个面的 6 个顶点从【面外侧看】必须是逆时针，
  // 否则会被 raylib 默认的背面剔除干掉（表现为这个面消失／能看穿）。
  // 方块占据 [x, x+1] × [y, y+1] × [z, z+1] 的空间。
  switch (faceIndex)
  {
  case FACE_TOP: // 顶面 Y+ (y + 1)
    // 三角形 1
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    // 三角形 2
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    break;

  case FACE_BOTTOM: // 底面 Y- (y + 0)
    // 三角形 1
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    // 三角形 2
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    break;

  case FACE_RIGHT: // 右面 X+ (x + 1)
    // 三角形 1
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    // 三角形 2
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    break;

  case FACE_LEFT: // 左面 X- (x + 0)
    // 三角形 1
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    // 三角形 2
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    break;

  case FACE_FRONT: // 前面 Z+ (z + 1)
    // 三角形 1
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    // 三角形 2
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z + 1);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z + 1);
    break;

  case FACE_BACK: // 后面 Z- (z + 0)
    // 三角形 1
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    // 三角形 2
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y + 1);
    vertices.push_back(z);
    vertices.push_back(x + 1);
    vertices.push_back(y);
    vertices.push_back(z);
    break;
  }

  // 3. 写入 6 个 UV 坐标
  // 按 (方块类型, 面朝向) 从图集里查出这一格
  const UVRect uv = GetBlockUV(blockType, faceIndex);

  // 一格的四个角，按 raylib 的 UV 约定（v 轴向下）：
  //   0 = 左上(u0,v0)  1 = 右上(u1,v0)  2 = 右下(u1,v1)  3 = 左下(u0,v1)
  const float us[4] = {uv.u0, uv.u1, uv.u1, uv.u0};
  const float vs[4] = {uv.v0, uv.v0, uv.v1, uv.v1};

  // 上面 switch 里写的 6 个顶点，其实是同一个四边形的：
  //   三角形1 = (角0, 角1, 角2)    三角形2 = (角0, 角2, 角3)
  // 但每个面的绕行方向不同，所以 UV 也要按各自方向重排，
  // 否则贴图会在某些面上上下颠倒 / 左右镜像。
  static constexpr int FACE_UV_ORDER[6][4] = {
      {3, 2, 1, 0}, // FACE_TOP    (+Y)
      {0, 1, 2, 3}, // FACE_BOTTOM (-Y)
      {3, 0, 1, 2}, // FACE_RIGHT  (+X)
      {3, 2, 1, 0}, // FACE_LEFT   (-X)
      {2, 3, 0, 1}, // FACE_FRONT  (+Z)
      {3, 0, 1, 2}, // FACE_BACK   (-Z)
  };
  static constexpr int QUAD_CORNER[6] = {0, 1, 2, 0, 2, 3};

  for (int k = 0; k < 6; k++)
  {
    const int c = FACE_UV_ORDER[faceIndex][QUAD_CORNER[k]];
    texcoords.push_back(us[c]);
    texcoords.push_back(vs[c]);
  }
}

UVRect GetBlockUV(BlockType blockType, int faceIndex)
{
  // 图集布局 assets/atlas.png（256x256，每格 TILE_SIZE = 16）：
  //   row 0:  col0 = 泥土   col1 = 草顶   col2 = 草侧   col3 = 石头
  int col = 3;
  switch (blockType)
  {
  case BlockType::DIRT:
    col = 0;
    break;
  case BlockType::GRASS:
    if (faceIndex == FACE_TOP)
      col = 1; // 顶面：纯草
    else if (faceIndex == FACE_BOTTOM)
      col = 0; // 底面：露出泥土
    else
      col = 2; // 侧面：上面一条草 + 下面泥土
    break;
  case BlockType::STONE:
  default:
    col = 3;
    break;
  }
  return g_atlas.GetUV(col, 0);
}

void Chunk::buildMesh()
{
  if (this->m_hasMesh)
  {
    UnloadMesh(m_mesh);
    // 关键：UnloadMesh 是按值传参的，不会清空 m_mesh ——
    // 不清的话 m_mesh 里留着已经释放的指针和旧的 vaoId，
    // 再调 UploadMesh 会因为 "vaoId > 0" 直接返回，什么都不传。
    m_mesh = Mesh{};
    m_hasMesh = false;
  }

  std::vector<float> tempVertices;
  std::vector<float> tempNormals;
  std::vector<float> tempTexcoords;

  // 预计算当前区块的世界坐标偏移
  int baseWorldX = this->m_coord.x * CHUNK_WIDTH;
  int baseWorldZ = this->m_coord.z * CHUNK_DEPTH;

  for (int x = 0; x < CHUNK_WIDTH; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_DEPTH; z++)
      {

        BlockType blockType = this->getBlock(x, y, z);
        if (blockType == BlockType::AIR)
          continue;

        // 计算世界坐标
        int worldX = baseWorldX + x;
        int worldY = y;
        int worldZ = baseWorldZ + z;

        for (int i = 0; i < 6; i++)
        {
          const Direction &dir = DIRECTIONS[i];
          const int nx = x + dir.dx;
          const int ny = y + dir.dy;
          const int nz = z + dir.dz;

          bool isAir = false;

          // 检查是否在当前 Chunk 局部范围内
          if (nx >= 0 && nx < CHUNK_WIDTH &&
              ny >= 0 && ny < CHUNK_HEIGHT &&
              nz >= 0 && nz < CHUNK_DEPTH)
          {
            isAir = (this->getBlock(nx, ny, nz) == BlockType::AIR);
          }
          else
          {
            // 超出 Chunk 边界时的处理
            if (ny >= CHUNK_HEIGHT || ny < 0)
            {
              // Y 轴超界：原逻辑中超出世界顶/底部直接视为空气露出表面
              isAir = true;
            }
            else
            {
              // X / Z 轴跨 Chunk：必须问 World（世界坐标）。
              // 不能写裸的 getBlock —— 那会解析成 Chunk::getBlock（本地 0~15 坐标），
              // 把世界坐标喂进去就是越界读内存。
              isAir = (m_world == nullptr) ||
                      (m_world->getBlock(worldX + dir.dx, worldY + dir.dy,
                                         worldZ + dir.dz) == BlockType::AIR);
            }
          }

          if (isAir)
          {
            AddFaceData(tempVertices, tempNormals, tempTexcoords, i, x, y, z, blockType);
          }
        }
      }
    }
  }

  // ---- 把攒好的数据上传成 GPU 上的 mesh ----
  if (tempVertices.empty())
  {
    // 整个区块都是空气：没有东西可画
    m_hasMesh = false;
    m_isDirty = false;
    return;
  }

  const int vertexCount = (int)(tempVertices.size() / 3);

  // raylib 的 Mesh 用的是三个【分离】的数组，不是交错布局
  m_mesh = Mesh{};
  m_mesh.vertexCount = vertexCount;
  m_mesh.triangleCount = vertexCount / 3;

  // 必须用 MemAlloc 分配：UnloadMesh() 内部用 RL_FREE(== free) 释放这些指针
  m_mesh.vertices = (float *)MemAlloc(tempVertices.size() * sizeof(float));
  m_mesh.texcoords = (float *)MemAlloc(tempTexcoords.size() * sizeof(float));
  m_mesh.normals = (float *)MemAlloc(tempNormals.size() * sizeof(float));

  std::copy(tempVertices.begin(), tempVertices.end(), m_mesh.vertices);
  std::copy(tempTexcoords.begin(), tempTexcoords.end(), m_mesh.texcoords);
  std::copy(tempNormals.begin(), tempNormals.end(), m_mesh.normals);

  // 上传到 GPU。第二个参数 false = 静态数据（之后每帧不会再改）
  UploadMesh(&m_mesh, false);

  m_hasMesh = true;
  m_isDirty = false;
}

BlockType Chunk::getBlock(int localX, int localY, int localZ) const
{
  assert(localX >= 0 && localX < CHUNK_WIDTH);
  assert(localY >= 0 && localY < CHUNK_HEIGHT);
  assert(localZ >= 0 && localZ < CHUNK_DEPTH);

  const int index =
      localY * (CHUNK_WIDTH * CHUNK_DEPTH) + localZ * CHUNK_WIDTH + localX;

  return m_blocks[index];
}

void Chunk::setBlock(int localX, int localY, int localZ, BlockType blockType)
{
  assert(localX >= 0 && localX < CHUNK_WIDTH);
  assert(localY >= 0 && localY < CHUNK_HEIGHT);
  assert(localZ >= 0 && localZ < CHUNK_DEPTH);

  const int index =
      localY * (CHUNK_WIDTH * CHUNK_DEPTH) + localZ * CHUNK_WIDTH + localX;

  if (m_blocks[index] != blockType)
  {
    m_blocks[index] = blockType;
    m_isDirty = true;
  }
}

void Chunk::update()
{
  // 只有被标脏的区块才重建 mesh（否则每帧 6.5 万个方块会卡死）
  if (m_isDirty)
  {
    buildMesh();
  }
}

void Chunk::Draw()
{
  if (!m_hasMesh)
  {
    return;
  }

  // buildMesh 生成的是【区块局部坐标】的顶点，
  // 所以绘制时平移到这个区块在世界里的位置。
  const Matrix transform =
      MatrixTranslate((float)(m_coord.x * CHUNK_WIDTH), 0.0f,
                      (float)(m_coord.z * CHUNK_DEPTH));

  DrawMesh(m_mesh, g_blockMaterial, transform);
}
