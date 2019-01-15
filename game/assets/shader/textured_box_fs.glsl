#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D diffuseTexture;

//uniform vec3 color;

void main(void)
{	
	FragColor = texture(diffuseTexture, TexCoord);
};