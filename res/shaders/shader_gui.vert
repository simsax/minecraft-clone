#version 450 core

layout(location = 0) in vec3 in_VertexCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * vec4(in_VertexCoord.x, in_VertexCoord.z, 0.0f, 1.0f);
	v_TexCoord = in_VertexCoord.xz;
}
