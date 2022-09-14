#version 450 core

in vec4 vColor;
in vec4 fragPos;
out vec4 texColor;

uniform vec4 u_Color;
uniform vec4 u_FogColor;
//vec4 u_FogColor = vec4(1);
uniform float u_LowerLimit;
const float upperLimit = 300.0f + u_LowerLimit;
//const float lowerLimit = 540.0f;
//const float upperLimit = 740.0f;

void main() {
    float factor = (fragPos.y - u_LowerLimit) / (upperLimit - u_LowerLimit);
    factor = clamp(factor, 0.0f, 1.0f);
    texColor = mix(u_FogColor, u_Color, factor);
//    texColor = vColor;
}
