#version 450 core

layout(location = 0) in vec3 in_VertexCoord;
layout(location = 1) in vec3 in_Color;

uniform mat4 u_MVP;
out vec4 color;

void main() {
	gl_Position = u_MVP * vec4(in_VertexCoord.x, in_VertexCoord.y, in_VertexCoord.z, 1.0f);
	color = vec4(in_Color, 1.0);
}
