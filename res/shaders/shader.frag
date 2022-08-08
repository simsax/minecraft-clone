#version 450 core

in vec2 v_TexCoord;
in float visibility;

out vec4 texColor;

uniform sampler2D u_Texture;
uniform vec3 u_SkyColor;

void main() {
	texColor = texture(u_Texture, v_TexCoord);
//	texColor = mix(vec4(u_SkyColor, 1.0f), texColor, visibility);
}
