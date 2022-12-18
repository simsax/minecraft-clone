#version 450 core

layout(location = 0) in vec3 in_VertexCoord;

out vec2 v_TexCoord;

uniform mat4 u_VP;
uniform mat4 u_V;
uniform vec3 u_BillboardPos;
uniform vec2 u_BillboardSize;

const vec3 cameraRight = {u_V[0][0], u_V[1][0], u_V[2][0]};
const vec3 cameraUp = {u_V[0][1], u_V[1][1], u_V[2][1]};

// still have to declare it as a separate shader in the quad renderer
void main() {
	gl_Position = u_VP *  vec4(u_BillboardPos + cameraRight * in_VertexCoord.x * u_BillboardSize.x +
								cameraUp * in_VertexCoord.z * u_BillboardSize.y, 1.0);
	v_TexCoord = in_VertexCoord.xz;
}
