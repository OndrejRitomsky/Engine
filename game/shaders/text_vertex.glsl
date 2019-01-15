#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec2 offset;

out vec2 TexCoords;

uniform M44 mp;

void main()
{
	gl_Position = mp * vec4(position.x + offset.x, position.y + offset.y, 0.0, 1.0);
	TexCoords = texCoord;
} 