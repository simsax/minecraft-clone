#version 330 core

in vec2 v_TexCoord;

out vec4 texColor;

uniform sampler2D u_Texture;
uniform vec4 u_TexColor;

void main() {
	texColor = u_TexColor * texture(u_Texture, v_TexCoord);
};