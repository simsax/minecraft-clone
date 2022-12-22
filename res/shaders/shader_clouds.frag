#version 450 core

in vec2 v_TexCoord;

out vec4 texColor;

uniform sampler2D u_Texture;
uniform float u_Time;
uniform float u_Color;

void main() {
	texColor = texture(u_Texture, vec2(v_TexCoord.x + u_Time, v_TexCoord.y)) * vec4(u_Color);
	if (texColor.a == 0.0)
		discard;
}
