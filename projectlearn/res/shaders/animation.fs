#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
in vec2 TexCoords;
uniform vec3 girlColor;

void main()
{

    FragColor = texture(texture_diffuse1,TexCoords) * vec4(girlColor,1);
} 