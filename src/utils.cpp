#include "raylib.h"
#include "rlgl.h"

void DrawCubeTexture(Texture2D texture, Vector3 position, float width,
                     float height, float length, Color color)
{
  float x = position.x;
  float y = position.y;
  float z = position.z;

  // Set desired texture to be enabled while drawing following vertex data
  rlSetTexture(texture.id);

  // Vertex data transformation can be defined with the commented lines,
  // but in this example we calculate the transformed vertex data directly when
  // calling rlVertex3f() rlPushMatrix(); NOTE: Transformation is applied in
  // inverse order (scale -> rotate -> translate) rlTranslatef(2.0f, 0.0f,
  // 0.0f); rlRotatef(45, 0, 1, 0); rlScalef(2.0f, 2.0f, 2.0f);

  rlBegin(RL_QUADS);
  rlColor4ub(color.r, color.g, color.b, color.a);
  // Front Face
  rlNormal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2,
             z + length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2,
             z + length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2,
             z + length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2,
             z + length / 2); // Top Left Of The Texture and Quad
  // Back Face
  rlNormal3f(0.0f, 0.0f, -1.0f); // Normal Pointing Away From Viewer
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2,
             z - length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2,
             z - length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2,
             z - length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2,
             z - length / 2); // Bottom Left Of The Texture and Quad
  // Top Face
  rlNormal3f(0.0f, 1.0f, 0.0f); // Normal Pointing Up
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2,
             z - length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x - width / 2, y + height / 2,
             z + length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x + width / 2, y + height / 2,
             z + length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2,
             z - length / 2); // Top Right Of The Texture and Quad
  // Bottom Face
  rlNormal3f(0.0f, -1.0f, 0.0f); // Normal Pointing Down
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x - width / 2, y - height / 2,
             z - length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x + width / 2, y - height / 2,
             z - length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2,
             z + length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2,
             z + length / 2); // Bottom Right Of The Texture and Quad
  // Right face
  rlNormal3f(1.0f, 0.0f, 0.0f); // Normal Pointing Right
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2,
             z - length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2,
             z - length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x + width / 2, y + height / 2,
             z + length / 2); // Top Left Of The Texture and Quad
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x + width / 2, y - height / 2,
             z + length / 2); // Bottom Left Of The Texture and Quad
  // Left Face
  rlNormal3f(-1.0f, 0.0f, 0.0f); // Normal Pointing Left
  rlTexCoord2f(0.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2,
             z - length / 2); // Bottom Left Of The Texture and Quad
  rlTexCoord2f(1.0f, 0.0f);
  rlVertex3f(x - width / 2, y - height / 2,
             z + length / 2); // Bottom Right Of The Texture and Quad
  rlTexCoord2f(1.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2,
             z + length / 2); // Top Right Of The Texture and Quad
  rlTexCoord2f(0.0f, 1.0f);
  rlVertex3f(x - width / 2, y + height / 2,
             z - length / 2); // Top Left Of The Texture and Quad
  rlEnd();
  // rlPopMatrix();

  rlSetTexture(0);
}

void DrawBlockFace(Texture2D texture, Vector3 position, float width,
                   float height, float length, Vector3 direction, Color color)
{
  float x = position.x;
  float y = position.y;
  float z = position.z;

  float halfWidth = width / 2.0f;
  float halfHeight = height / 2.0f;
  float halfLength = length / 2.0f;

  Vector3 vertices[4];
  Vector3 normal;

  // Choose the four vertices that lie on the requested face.
  // position is the center of the block, so each face is half a block away.
  if (direction.y > 0.5f)
  {
    // Top (+Y)
    normal = {0.0f, 1.0f, 0.0f};
    vertices[0] = {x - halfWidth, y + halfHeight, z + halfLength};
    vertices[1] = {x + halfWidth, y + halfHeight, z + halfLength};
    vertices[2] = {x + halfWidth, y + halfHeight, z - halfLength};
    vertices[3] = {x - halfWidth, y + halfHeight, z - halfLength};
  }
  else if (direction.y < -0.5f)
  {
    // Bottom (-Y)
    normal = {0.0f, -1.0f, 0.0f};
    vertices[0] = {x - halfWidth, y - halfHeight, z - halfLength};
    vertices[1] = {x + halfWidth, y - halfHeight, z - halfLength};
    vertices[2] = {x + halfWidth, y - halfHeight, z + halfLength};
    vertices[3] = {x - halfWidth, y - halfHeight, z + halfLength};
  }
  else if (direction.x > 0.5f)
  {
    // Right (+X)
    normal = {1.0f, 0.0f, 0.0f};
    vertices[0] = {x + halfWidth, y - halfHeight, z - halfLength};
    vertices[1] = {x + halfWidth, y + halfHeight, z - halfLength};
    vertices[2] = {x + halfWidth, y + halfHeight, z + halfLength};
    vertices[3] = {x + halfWidth, y - halfHeight, z + halfLength};
  }
  else if (direction.x < -0.5f)
  {
    // Left (-X)
    normal = {-1.0f, 0.0f, 0.0f};
    vertices[0] = {x - halfWidth, y - halfHeight, z + halfLength};
    vertices[1] = {x - halfWidth, y + halfHeight, z + halfLength};
    vertices[2] = {x - halfWidth, y + halfHeight, z - halfLength};
    vertices[3] = {x - halfWidth, y - halfHeight, z - halfLength};
  }
  else if (direction.z > 0.5f)
  {
    // Front (+Z)
    normal = {0.0f, 0.0f, 1.0f};
    vertices[0] = {x - halfWidth, y - halfHeight, z + halfLength};
    vertices[1] = {x + halfWidth, y - halfHeight, z + halfLength};
    vertices[2] = {x + halfWidth, y + halfHeight, z + halfLength};
    vertices[3] = {x - halfWidth, y + halfHeight, z + halfLength};
  }
  else if (direction.z < -0.5f)
  {
    // Back (-Z)
    normal = {0.0f, 0.0f, -1.0f};
    vertices[0] = {x - halfWidth, y - halfHeight, z - halfLength};
    vertices[1] = {x - halfWidth, y + halfHeight, z - halfLength};
    vertices[2] = {x + halfWidth, y + halfHeight, z - halfLength};
    vertices[3] = {x + halfWidth, y - halfHeight, z - halfLength};
  }
  else
  {
    // Only the six axis-aligned directions are supported.
    return;
  }

  rlSetTexture(texture.id);
  rlBegin(RL_QUADS);
  rlColor4ub(color.r, color.g, color.b, color.a);
  rlNormal3f(normal.x, normal.y, normal.z);

  const float textureCoordinates[4][2] = {
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
  };

  for (int i = 0; i < 4; ++i)
  {
    rlTexCoord2f(textureCoordinates[i][0], textureCoordinates[i][1]);
    rlVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
  }

  rlEnd();
  rlSetTexture(0);
}

Texture2D LoadTexturePNG(char const *fileName)
{
  Texture2D texture = LoadTexture(fileName);
  if (texture.id == 0)
  {
    TraceLog(LOG_WARNING, "Failed to load texture: %s", fileName);
  }
  SetTextureFilter(texture, TEXTURE_FILTER_POINT);
  return texture;
}
