#include "block.h"
#include <map>
#include <stdexcept>

std::map<BlockType, BlockProperties> BlockRegistry::s_propertiesMap;

void BlockRegistry::Initialize() {
  s_propertiesMap[BlockType::AIR] = {BlockType::AIR, "Air", true};
  s_propertiesMap[BlockType::STONE] = {BlockType::STONE, "Stone", false};
  s_propertiesMap[BlockType::GRASS] = {BlockType::GRASS, "Grass", false};
}

const BlockProperties &BlockRegistry::Get(BlockType id) {
  auto it = s_propertiesMap.find(id);
  if (it != s_propertiesMap.end()) {
    return it->second;
  }
  throw std::runtime_error(
      "Attempted to get properties for an unregistered block ID.");
}
