#version 450 core

in vec2 v_TexCoord;

out vec4 texColor;

uniform sampler2D u_Texture;
uniform float u_Alpha;

void main() {
	texColor = texture(u_Texture, v_TexCoord) * vec4(1.0f, 1.0f, 1.0f, u_Alpha);
	if (texColor.a == 0.0)
		discard;
}
