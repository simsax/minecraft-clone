#version 450 core

in vec2 v_TexCoord;
in float visibility;
in vec3 normal;
in vec3 fragPos;
in float sunLight;
in float redLight;
in float greenLight;
in float blueLight;

out vec4 texColor;

uniform sampler2D u_Texture;
uniform vec3 u_SkyColor;
uniform vec3 u_SunColor;
uniform vec3 u_ViewPos;

float specularStrength = 0.1f;
int shininess = 16;

const float gamma = 1.0 / 2.2;

void main() {
	texColor = texture(u_Texture, v_TexCoord) * vec4(redLight, greenLight, blueLight, 1.0f);
	if (texColor.a < 0.01)
		discard;
	texColor = mix(vec4(u_SkyColor, 1.0f), texColor, visibility);
}
