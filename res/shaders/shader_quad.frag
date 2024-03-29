#version 450 core

in vec2 v_TexCoord;

out vec4 texColor;

uniform sampler2D u_Texture;

void main() {
	texColor = texture(u_Texture, v_TexCoord);
	if (texColor.a == 0.0)
		discard;
}
