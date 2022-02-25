#shader vertex
#version 330 core

layout(location = 0) in vec4 position; // location corresponds to id of glVertexAttribPointer
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 color;

out vec2 v_TexCoord; // output data from vertex shader to fragment shader
out vec4 v_Color;

uniform mat4 u_MVP; // model view projection matrix

void main() {
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
	v_Color = color;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 texColor;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D u_Texture;

void main() {
	texColor = texture(u_Texture, v_TexCoord);
};