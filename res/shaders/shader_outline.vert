#version 330 core

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
	float x = float((in_VertexCoord & 0x7800000u) >> 23) + u_ChunkPos.x;
	float y = float((in_VertexCoord & 0x7F8000u) >> 15) + u_ChunkPos.y;
	float z = float((in_VertexCoord & 0x7800u) >> 11) + u_ChunkPos.z;
	gl_Position = u_MVP * vec4(x, y, z, 1.0f);
	vec2 texCoord = vec2((in_VertexCoord & 0x3C0u) >> 6u,
	(in_VertexCoord & 0x3Cu) >> 2u);
	uint index = in_VertexCoord & 3u;
	v_TexCoord = (texCoord + texCoords[index]) * offset;
}
