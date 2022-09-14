#version 450 core

layout(location = 0) in vec3 in_VertexCoord;

uniform mat4 u_MVP;
uniform vec4 u_Colors[4];

out vec4 vColor;

void main() {
    gl_Position = u_MVP * vec4(in_VertexCoord.x, in_VertexCoord.z, 0.0f, 1.0f);
    vColor = u_Colors[gl_VertexID];
}
