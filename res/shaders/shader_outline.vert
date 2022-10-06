#version 450 core

layout(location = 0) in uint in_VertexCoord;

uniform mat4 u_MVP;
uniform vec3 u_ChunkPos;

void main() {
	float x = float(in_VertexCoord >> 24 & 0x1Fu) + u_ChunkPos.x;
	float y = float(in_VertexCoord >> 15 & 0x1FFu) + u_ChunkPos.y;
	float z = float(in_VertexCoord >> 10 & 0x1Fu) + u_ChunkPos.z;
	gl_Position = u_MVP * vec4(x, y, z, 1.0f);

}
