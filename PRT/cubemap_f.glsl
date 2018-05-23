#version 430
#extension GL_NV_shadow_samplers_cube : enable
in vec3 texcoord;
out vec4 fragColor;
uniform samplerCube cubemap;
 
void main (void) 
{
    fragColor = textureCube(cubemap, texcoord);
}