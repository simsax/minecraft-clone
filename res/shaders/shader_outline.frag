#version 450 core

//in vec2 v_TexCoord;

out vec4 texColor;

//uniform sampler2D u_Texture;
uniform float u_Visibility;
//uniform bool u_Outline;

void main() {
    //	if (u_Outline)
    //		texColor = vec4(0, 0, 0, 1);
    //	else
    //		texColor = vec4(1, 0, 0, 1);
    //	texColor = mix(vec4(1,1,1,1), texture(u_Texture, v_TexCoord), u_Visibility);
    	texColor = mix(vec4(1,1,1,0.5), vec4(1,1,1,0.1), u_Visibility);
}
