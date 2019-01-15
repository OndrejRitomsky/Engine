#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
// layout (location = 2) in M44 mvp;

uniform M44 mvp;

out vec2 TexCoord;

void main() {
	gl_Position =  mvp * vec4(position.x , position.y, 0.0, 1.0);
	TexCoord = texCoord;
} 