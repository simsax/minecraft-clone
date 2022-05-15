#shader vertex
#version 330 core

// this layout will change
layout(location = 0) in vec4 aPos;
layout(location = 1) in uint aTexCoord;

out vec2 v_TexCoord;
uniform mat4 u_MVP; 

const float offset = 0.0625f;

vec2 texCoords[4] = vec2[4](
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f),
	vec2(0.0f, 1.0f)
);

void main() {
	gl_Position = u_MVP * aPos;
	vec2 texCoord = vec2((aTexCoord & 0x3C0u) >> 6u,
						 (aTexCoord & 0x3Cu) >> 2u);
	uint index = aTexCoord & 3u;
	v_TexCoord = (texCoord + texCoords[index]) * offset;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 texColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main() {
	texColor = texture(u_Texture, v_TexCoord);
	//texColor = vec4(0.0,0.0,0.0,1.0);
};