#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 FragColor;


void main()
{
    FragColor = (ParticleColor);
}