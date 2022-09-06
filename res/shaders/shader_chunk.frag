#version 450 core

in vec2 v_TexCoord;
in float visibility;
in vec3 normal;
in vec3 fragPos;

out vec4 texColor;

uniform sampler2D u_Texture;
uniform vec3 u_SkyColor;
uniform vec3 u_SunColor;

const float ambientStrength = 0.1f;
const vec3 lightPos = vec3(0, 1000, 1000);

void main() {
	vec3 ambient = u_SunColor * ambientStrength;
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffStrength = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = u_SunColor * diffStrength;
	texColor = texture(u_Texture, v_TexCoord) * vec4(ambient + diffuse, 1.0f);
	if (texColor.a < 0.01)
		discard;
	texColor = mix(vec4(u_SkyColor, 1.0f), texColor, visibility);
}
