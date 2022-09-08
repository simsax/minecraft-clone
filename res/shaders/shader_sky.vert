#version 450 core

layout(location = 0) in vec3 in_VertexCoord;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(in_VertexCoord, 1.0f);
}
