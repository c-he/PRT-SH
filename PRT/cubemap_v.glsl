#version 430
#extension GL_NV_shadow_samplers_cube : enable
in vec3 vertex;
uniform mat4 PVM;
out vec3 texcoord;

void main () 
{
	texcoord = 0.00025 * vec3(vertex.xy,-vertex.z);
	gl_Position = PVM * vec4 (vertex, 1.0);
}