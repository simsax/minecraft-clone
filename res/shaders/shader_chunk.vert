#version 450 core

layout(location = 0) in uint in_VertexCoord;
layout(location = 1) in uint in_Light;

out vec2 v_TexCoord;
out float visibility;
out vec3 fragPos;
out vec3 normal;

out float sunLight;
out float redLight;
out float greenLight;
out float blueLight;

uniform mat4 u_MVP;
uniform mat4 u_MV;
uniform vec3 u_ChunkPos;
uniform vec3 u_LightDir;
uniform vec4 u_AmbientOcclusion;

const float offset = 0.0625f;
const float density = 0.003f;
const float gradient = 10.0f;
const float ambient = 0.3f;

const vec2 texCoords[4] = vec2[4](
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f),
	vec2(0.0f, 1.0f)
);

const vec3 normals[6] = vec3[6](
	vec3(1.0f, 0.0f, 0.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, -1.0f)
);

//const vec4 ambientOcclusion = vec4(1, 0.8, 0.6, 0.3);

void main() {
	float x = float(in_VertexCoord >> 24 & 0x1Fu) + u_ChunkPos.x;
	float y = float(in_VertexCoord >> 15 & 0x1FFu) + u_ChunkPos.y;
	float z = float(in_VertexCoord >> 10 & 0x1Fu) + u_ChunkPos.z;
	uint normalIndex = in_VertexCoord >> 29 & 0x7u;
	vec3 upVec = normals[2];
	normal = normals[normalIndex];
	fragPos = vec3(x, y, z);

	gl_Position = u_MVP * vec4(x, y, z, 1.0f);
	vec4 posRelToCam = u_MV * vec4(x, y, z, 1.0f);

	vec2 texCoord = vec2(in_VertexCoord >> 6 & 0xFu,
						 in_VertexCoord >> 2 & 0xFu);
	uint index = in_VertexCoord & 0x3u;
	v_TexCoord = (texCoord + texCoords[index]) * offset;

	float distance = length(posRelToCam.xyz);
	visibility = exp(-pow((distance*density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	// sun has same dir for all fragments, so I can do diffuse calculation on vertex shader
	vec3 lightDir = normalize(-u_LightDir);
	float diffStrength = min(max(dot(lightDir, upVec), 0.0f), 0.9f);
	float sunStrength = diffStrength * max(dot(lightDir, normal), 0.0f);

	sunLight = sunStrength * float(in_Light >> 12 & 0xFu) / 15.0 + ambient;

	uint aoIndex = in_Light >> 16 & 0xFu;
	float ao = u_AmbientOcclusion[aoIndex];

	redLight =   clamp(float(in_Light >> 8 & 0xFu) / 15.0 + sunLight, 0.0, 1.0) * ao;
	greenLight = clamp(float(in_Light >> 4 & 0xFu) / 15.0 + sunLight, 0.0, 1.0) * ao;
	blueLight =  clamp(float(in_Light & 0xFu) / 15.0 + sunLight, 0.0, 1.0) * ao;
}
