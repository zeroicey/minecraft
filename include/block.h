#pragma once
#include <cstdint>
#include <map>
#include <string>

enum BlockID : uint8_t { AIR, STONE, DIRT, GRASS };

struct BlockProperties {
  BlockID id;
  std::string name;
  bool is_transparent;
};

class BlockRegistry {
public:
  static void Initialize();
  static const BlockProperties &Get(BlockID id);

private:
  static std::map<BlockID, BlockProperties> s_propertiesMap;
};
