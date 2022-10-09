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
// uniform vec3 u_LightDir;

float specularStrength = 0.1f;
int shininess = 16;

const float gamma = 1.0 / 2.2;

void main() {
	// vec3 ambient = u_SunColor * u_AmbientStrength;
  //  vec3 lightDir = normalize(-u_LightDir);
  //  vec3 norm = normalize(normal);
  //  float diffStrength = max(dot(lightDir, norm), 0.0f);
  //  vec3 diffuse = u_SunColor * diffStrength;
  //  vec3 viewDir = normalize(u_ViewPos - fragPos);
  //  vec3 halfwayDir = normalize(viewDir + lightDir);
  //  float spec = pow(max(dot(halfwayDir, norm), 0.0f), shininess);
  //  vec3 specular = u_SunColor * specularStrength * spec;
	// if (u_IsDay) {
	// 	texColor = texture(u_Texture, v_TexCoord) * vec4(ambient + diffuse + specular, 1.0f);
	// }
	// else
	// 	texColor = texture(u_Texture, v_TexCoord) * vec4(ambient + diffuse + specular, 1.0f) * vec4(0.5f, 0.5f, 0.5f, 1.0f);
  texColor = texture(u_Texture, v_TexCoord) * vec4(redLight, greenLight, blueLight, 1.0f);
	// texColor.x = clamp(texColor.x + redLight, 0.0, 1.0);
	// texColor.y = clamp(texColor.y + greenLight, 0.0, 1.0);
	// texColor.z = clamp(texColor.z + blueLight, 0.0, 1.0);
  // texColor *= vec4(redLight, greenLight, blueLight, 1.0f);
	if (texColor.a < 0.01)
		discard;
	texColor = mix(vec4(u_SkyColor, 1.0f), texColor, visibility);
	// texColor.rgb = pow(texColor.rgb, vec3(gamma));
}
