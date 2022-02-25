#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // correspond to id of glVertexAttribPointer
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 color;
layout(location = 2) in float texId;

out vec2 v_TexCoord; // output data from vertex shader to fragment shader
out vec4 v_Color;
out float v_TexId;

uniform mat4 u_MVP; // model view projection matrix

void main() {
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
	v_Color = color;
	v_TexId = texId;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 texColor;
//layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexId;

//uniform vec4 u_Color;
uniform sampler2D u_Textures[2];

void main() {
	int index = int(v_TexId);
	texColor = texture(u_Textures[index], v_TexCoord);
	//texColor = vec4(v_TexId, v_TexId, v_TexId, 1.0);
	//color = v_Color;
};