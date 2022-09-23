#version 450 core

layout(location = 0) in vec2 in_VertexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec4 fragPos;

void main() {
    gl_Position = u_MVP * vec4(in_VertexCoord, 0.0f, 1.0f);
    fragPos = u_Model * vec4(in_VertexCoord, 0.0f, 1.0f);
}
