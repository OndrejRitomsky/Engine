#version 330 core
layout (location = 0) in vec2 position;

uniform M44 mvp;

void main() {
	gl_Position =  mvp * vec4(position.x , position.y, 0.0, 1.0);
} 