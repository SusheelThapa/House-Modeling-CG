#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;
uniform vec3 skyColor;

void main()
{    
    color = texture(skybox, TexCoords) * vec4(skyColor,1.f);
}