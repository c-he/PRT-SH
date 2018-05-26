#version 430
#extension GL_NV_shadow_samplers_cube : enable

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube cubemap;

void main()
{
    FragColor = textureCube(cubemap, TexCoords);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}