#include "texture_atlas.h"
#include "config.h"

TextureAtlas::TextureAtlas(const std::string& filePath) {
  Load(filePath, TILE_SIZE, ATLAS_SIZE);
}

TextureAtlas::~TextureAtlas() {
  Unload();
}

TextureAtlas::TextureAtlas(TextureAtlas&& other) noexcept 
    : m_texture(other.m_texture), 
      m_loaded(other.m_loaded) {
    other.m_texture.id = 0;
    other.m_loaded = false;
}

TextureAtlas& TextureAtlas::operator=(TextureAtlas&& other) noexcept {
    if (this != &other) {
        Unload();
        m_texture = other.m_texture;
        m_loaded = other.m_loaded;

        other.m_texture.id = 0;
        other.m_loaded = false;
    }
    return *this;
}

bool TextureAtlas::Load(const std::string& filePath, float tileSize, float atlasSize) {
    if (m_loaded) Unload();

    m_texture = LoadTexture(filePath.c_str());
    if (m_texture.id == 0) {
        TraceLog(LOG_ERROR, "TextureAtlas: Failed to load texture from %s", filePath.c_str());
        return false;
    }

    // 像素游戏必须设置点采样（最近邻插值）
    SetTextureFilter(m_texture, TEXTURE_FILTER_POINT);

    m_loaded = true;
    return true;
}

void TextureAtlas::Unload() {
    if (m_loaded) {
        UnloadTexture(m_texture);
        m_texture.id = 0;
        m_loaded = false;
    }
}

// 从内存加载：图集在编译期已经被塞进 exe（见 cmake/embed_binary.cmake），
// 所以运行时不需要 assets/ 目录，发布只要发一个 exe。
bool TextureAtlas::LoadFromMemory(const unsigned char* data, int dataSize) {
    if (m_loaded) Unload();

    if (data == nullptr || dataSize <= 0) {
        TraceLog(LOG_ERROR, "TextureAtlas: embedded image data is empty");
        return false;
    }

    // 第二个参数是文件类型，raylib 靠它选解码器（嵌入的是 PNG）
    Image image = LoadImageFromMemory(".png", data, dataSize);
    if (image.data == nullptr) {
        TraceLog(LOG_ERROR, "TextureAtlas: LoadImageFromMemory failed");
        return false;
    }

    m_texture = LoadTextureFromImage(image);
    UnloadImage(image); // 像素已经上传到 GPU 了，CPU 侧这份要还回去

    if (m_texture.id == 0) {
        TraceLog(LOG_ERROR, "TextureAtlas: LoadTextureFromImage failed");
        return false;
    }

    // 像素游戏必须设置点采样（最近邻插值）
    SetTextureFilter(m_texture, TEXTURE_FILTER_POINT);

    m_loaded = true;
    return true;
}

UVRect TextureAtlas::GetUV(int col, int row) const {
    // 0.5 像素微调收缩，防止远景下 GPU 线性采样溢出相邻格子
    const float halfPixel = 0.5f;

    UVRect uv;
    uv.u0 = (col * TILE_SIZE + halfPixel) / ATLAS_SIZE;
    uv.v0 = (row * TILE_SIZE + halfPixel) / ATLAS_SIZE;
    uv.u1 = ((col + 1.0f) * TILE_SIZE - halfPixel) / ATLAS_SIZE;
    uv.v1 = ((row + 1.0f) * TILE_SIZE - halfPixel) / ATLAS_SIZE;
    return uv;
}
