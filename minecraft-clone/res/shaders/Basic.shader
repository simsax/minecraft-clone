#shader vertex
#version 330 core

layout(location = 0) in vec4 aPos; // location corresponds to id of glVertexAttribPointer
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aOffset;

out vec2 v_TexCoord; // output data from vertex shader to fragment shader

uniform mat4 u_MVP; // model view projection matrix

void main() {
	gl_Position = u_MVP * vec4(aPos + aOffset);
	v_TexCoord = aTexCoord;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 texColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main() {
	texColor = texture(u_Texture, v_TexCoord);
};