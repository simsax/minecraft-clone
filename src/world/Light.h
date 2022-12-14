#include "ChunkManager.h"

using ChunkSet = std::unordered_set<ChunkCoord, hash_fn>;

namespace Light {
void LightPlacedBFS(std::queue<LightAddNode> lightQueue, Channel channel, ChunkSet& chunksToUpload);
void SunBFS(std::queue<LightAddNode> lightQueue, ChunkSet& chunksToUpload);
std::queue<LightAddNode> LightRemovedBFS(
    std::queue<LightRemNode> lightRemQueue, Channel channel, ChunkSet& chunksToUpload);
void UpdateLightPlacedQueue(std::queue<LightAddNode>& queue, uint8_t lightLevel, uint8_t i,
    uint8_t j, uint8_t k, Chunk* chunk, Channel channel, ChunkSet& chunksToUpload);
void UpdateLightRemovedQueue(std::queue<LightAddNode>& placeQueue,
    std::queue<LightRemNode>& removeQueue, int lightLevel, uint8_t i, uint8_t j, uint8_t k,
    Chunk* chunk, Channel channel, ChunkSet& chunksToUpload);
void UpdateOpaqueBlockLight(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z);
void UpdateLightOpaque(Chunk* chunk, Chunk* other, uint8_t x, uint8_t y, uint8_t z, uint8_t otherX,
    uint8_t otherY, uint8_t otherZ);
void AddSunLight(Chunk* chunk, ChunkSet& chunksToUpload);
void AddSunLightSimplified(Chunk* chunk, ChunkSet& chunksToUpload);
}

struct LightAddNode {
    LightAddNode(Chunk* c, uint16_t i)
        : chunk(c)
        , index(i)
    {
    }

    Chunk* chunk;
    uint16_t index;
};

struct LightRemNode {
    LightRemNode(Chunk* c, uint16_t i, uint8_t v)
        : chunk(c)
        , index(i)
        , val(v)
    {
    }

    Chunk* chunk;
    uint16_t index;
    uint8_t val;
};