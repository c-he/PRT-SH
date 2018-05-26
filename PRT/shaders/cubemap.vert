#version 430
#extension GL_NV_shadow_samplers_cube : enable

layout(location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
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