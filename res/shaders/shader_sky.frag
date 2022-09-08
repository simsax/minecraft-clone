#version 450 core

out vec4 texColor;

uniform vec4 u_Color;

void main() {
    texColor = u_Color;
}
