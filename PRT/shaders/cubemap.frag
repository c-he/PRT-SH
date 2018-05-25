#version 430
#extension GL_NV_shadow_samplers_cube : enable

in vec3 TexCoords;
out vec4 fragColor;

uniform samplerCube cubemap;
 
void main (void) 
{
    fragColor = textureCube(cubemap, TexCoords);
    // fragColor = vec4(1.0,0.0,0.0,1.0);
}