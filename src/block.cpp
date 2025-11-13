#include "block.h"
#include <map>
#include <stdexcept>

std::map<BlockID, BlockProperties> BlockRegistry::s_propertiesMap;

void BlockRegistry::Initialize() {
  s_propertiesMap[BlockID::AIR] = {BlockID::AIR, "Air", true};
  s_propertiesMap[BlockID::STONE] = {BlockID::STONE, "Stone", false};
  s_propertiesMap[BlockID::GRASS] = {BlockID::GRASS, "Grass", false};
}

const BlockProperties &BlockRegistry::Get(BlockID id) {
  auto it = s_propertiesMap.find(id);
  if (it != s_propertiesMap.end()) {
    return it->second;
  }
  throw std::runtime_error(
      "Attempted to get properties for an unregistered block ID.");
}
