#version 330 core

layout(location = 0) out vec4 texColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main() {
	texColor = texture(u_Texture, v_TexCoord);
	//texColor = vec4(0.0,0.0,0.0,1.0);
};