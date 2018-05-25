#version 430
#extension GL_NV_shadow_samplers_cube : enable

in vec3 vertex;
out vec3 TexCoords;

uniform mat4 PVM;

void main () 
{
	TexCoords = 0.00025 * vec3(vertex.xy,-vertex.z);
	gl_Position = PVM * vec4 (vertex, 1.0);
}

// layout (location = 0) in vec3 position;
// out vec3 TexCoords;

// uniform mat4 PVM;

// void main()
// {
//     vec4 pos = PVM * vec4(position, 1.0);
//     gl_Position = pos.xyww;
//     TexCoords = position;
// }  