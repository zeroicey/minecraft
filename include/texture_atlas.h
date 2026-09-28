#pragma once

#include "raylib.h"
#include <string>

// UV 矩形范围 (0.0f ~ 1.0f)
struct UVRect {
    float u0, v0; // 左上角
    float u1, v1; // 右下角
};

class TextureAtlas {
public:
    TextureAtlas() = default;
    TextureAtlas(const std::string& filePath);
    ~TextureAtlas();

    // 禁用拷贝构造与赋值（防止重复调用 UnloadTexture）
    TextureAtlas(const TextureAtlas&) = delete;
    TextureAtlas& operator=(const TextureAtlas&) = delete;

    // 允许移动构造（可选，方便容器管理）
    TextureAtlas(TextureAtlas&& other) noexcept;
    TextureAtlas& operator=(TextureAtlas&& other) noexcept;

    // 加载与卸载
    bool Load(const std::string& filePath, float tileSize, float atlasSize);
    // 从内存里的图片字节加载（用于编译进 exe 的资源，见 cmake/embed_binary.cmake）
    bool LoadFromMemory(const unsigned char* data, int dataSize);
    void Unload();

    // 根据图集行列坐标获取 UV 坐标（内置 0.5 像素防渗色）
    UVRect GetUV(int col, int row) const;

    // Getter
    const Texture2D& GetTexture() const { return m_texture; }
    bool IsLoaded() const { return m_loaded; }

private:
    Texture2D m_texture{ 0 };
    bool m_loaded = false;
};
