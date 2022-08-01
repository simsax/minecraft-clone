#version 330 core

layout(location = 0) in vec4 in_VertexCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * vec4(in_VertexCoord.xy, 0.0f, 1.0f);
	v_TexCoord = in_VertexCoord.zw;
};