#version 450 core

layout(location = 0) in vec3 in_VertexCoord;
layout(location = 1) in vec3 in_BillboardPos;
layout(location = 2) in vec2 in_BillboardSize;

out vec2 v_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_V;

const vec3 cameraRight = {u_V[0][0], u_V[1][0], u_V[2][0]};
const vec3 cameraUp = {u_V[0][1], u_V[1][1], u_V[2][1]};

void main() {
	gl_Position = u_MVP * vec4(in_BillboardPos + cameraRight * in_VertexCoord.x * in_BillboardSize.x +
								cameraUp * in_VertexCoord.z * in_BillboardSize.y, 1.0);
	v_TexCoord = in_VertexCoord.xz;
}
