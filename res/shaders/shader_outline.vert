#version 450 core

layout(location = 0) in uint in_VertexCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;
uniform vec3 u_ChunkPos;

const float offset = 0.0625f;
const vec2 texCoords[4] = vec2[4](
vec2(0.0f, 0.0f),
vec2(1.0f, 0.0f),
vec2(1.0f, 1.0f),
vec2(0.0f, 1.0f)
);

void main() {
	float x = float(in_VertexCoord >> 24 & 0x1Fu) + u_ChunkPos.x;
	float y = float(in_VertexCoord >> 15 & 0x1FFu) + u_ChunkPos.y;
	float z = float(in_VertexCoord >> 10 & 0x1Fu) + u_ChunkPos.z;
	gl_Position = u_MVP * vec4(x, y, z, 1.0f);
	vec2 texCoord = vec2(in_VertexCoord >> 6 & 0xFu, in_VertexCoord >> 2 & 0xFu);
	uint index = in_VertexCoord & 0x3u;
	v_TexCoord = (texCoord + texCoords[index]) * offset;
}
