#version 330 core

out vec4 FragColor;

uniform vec3 color;

void main() {
	vec3 r = vec3(1.0f,1.0f,1.f);
	FragColor =  vec4(color * r, 1.0f);
}
