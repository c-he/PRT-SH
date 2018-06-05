#version 430
#extension GL_NV_shadow_samplers_cube : enable

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube cubemap;

void main()
{
    // const float gamma = 2.2;
    // vec3 hdrColor = textureCube(cubemap, TexCoords).rgb;

    // // Reinhard Tone Mapping
    // vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // // Gamma Correction
    // mapped = pow(mapped, vec3(1.0 / gamma));

    // FragColor = vec4(mapped, 1.0);
    FragColor = textureCube(cubemap, TexCoords);
}