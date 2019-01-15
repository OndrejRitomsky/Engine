#version 330 core

layout (location = 0) in vec2 in_Position;

uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp * vec4(in_Position, 0.0, 1.0);
};