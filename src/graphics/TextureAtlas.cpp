#include "TextureAtlas.h"

namespace TextureAtlas {

    namespace {
        const std::unordered_map<Block, Coords> textureMap = {
                // top, side, bottom
                {Block::EMPTY,         {{0,  0},  {0,  0},  {0,  0}}},
                {Block::GRASS,         {{12, 3},  {3,  15}, {2,  15}}},
                {Block::DIRT,          {{2,  15}, {2,  15}, {2,  15}}},
                {Block::STONE,         {{1,  15}, {1,  15}, {1,  15}}},
                {Block::DIAMOND,       {{2,  12}, {2,  12}, {2,  12}}},
                {Block::GOLD,          {{0,  13}, {0,  13}, {0,  13}}},
                {Block::COAL,          {{2,  13}, {2,  13}, {2,  13}}},
                {Block::IRON,          {{1,  13}, {1,  13}, {1,  13}}},
                {Block::LEAVES,        {{15, 4},  {15, 4},  {15, 4}}},
                {Block::WOOD,          {{5,  14}, {4,  14}, {5,  14}}},
                {Block::SNOW,          {{2,  11}, {2,  11}, {2,  11}}},
                {Block::SNOWY_GRASS,   {{2,  11}, {4,  11}, {2,  15}}},
                {Block::WATER,         {{15, 3},  {15, 3},  {15, 3}}},
                {Block::SAND,          {{2,  14}, {2,  14}, {2,  14}}},
                {Block::GRAVEL,        {{3,  14}, {3,  14}, {3,  14}}},
                {Block::BEDROCK,       {{1,  14}, {1,  14}, {1,  14}}},
                {Block::FLOWER_BLUE,   {{12, 15}, {12, 15}, {12, 15}}},
                {Block::FLOWER_YELLOW, {{13, 15}, {13, 15}, {13, 15}}},
                {Block::BUSH,          {{14, 4},  {14, 4},  {14, 4}}},
                {Block::WET_DIRT,      {{6,  10}, {6,  10}, {6,  10}}},
                {Block::TERRACOTTA,    {{15, 14}, {15, 14}, {15, 14}}},
        };

        Texture texture(std::string(SOURCE_DIR) + "/res/textures/terrain.png");
    }

    Texture& GetTexture() {
        return texture;
    }

    void Init() {
        texture.Init();
    }

    Coords At(Block block) {
        return textureMap.at(block);
    }

}
