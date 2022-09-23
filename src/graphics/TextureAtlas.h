#pragma once

#include <unordered_map>
#include "../utils/Items.h"
#include "glm/glm.hpp"
#include "Texture.h"
#include "Config.h"

namespace TextureAtlas {

    struct Coords {
        Coords(glm::uvec2 top, glm::uvec2 side, glm::uvec2 bottom) :
                top({}), side({}), bottom({}) {
            for (int i = 0; i < 4; i++) {
                this->top[i] = (top[0] << 6) | (top[1] << 2) | i;
                this->side[i] = (side[0] << 6) | (side[1] << 2) | i;
                this->bottom[i] = (bottom[0] << 6) | (bottom[1] << 2) | i;
            }
        }

        std::array<uint32_t, 4> top;
        std::array<uint32_t, 4> side;
        std::array<uint32_t, 4> bottom;
    };

    void Init();
    Coords At(Block block);
    Texture& GetTexture();
};
