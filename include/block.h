#pragma once
#include <cstdint>
#include <map>
#include <string>

enum BlockType : uint8_t { AIR, STONE, DIRT, GRASS };

struct BlockProperties {
  BlockType type;
  std::string name;
  bool is_transparent;
};

class BlockRegistry {
public:
  static void Initialize();
  static const BlockProperties &Get(BlockType type);

private:
  static std::map<BlockType, BlockProperties> s_propertiesMap;
};
