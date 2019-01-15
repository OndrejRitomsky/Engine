#version 330 core


in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 color;

uniform float time;

#define PI 3.14159265358979323846

float SpiralRadius(vec2 uv, float scale, float change) {
	float angle = atan(uv.y, uv.x) + PI / 2.0;
	float fraction = angle / PI * scale + change;
	return fraction;
}

vec2 rotate2D(vec2 _st, float _angle){
	_st =  mat2(cos(_angle),-sin(_angle),
	sin(_angle),cos(_angle)) * _st;
	return _st;
}


void main() {
	float t = fract(time * 0.4) * 2.0;
	vec2 uv = TexCoord;

	uv -= vec2(0.5);
	uv = rotate2D(uv, t * PI); 

	float c = 0.0;
	float border = 0.03;
	float dist = length(uv);

	for (int i = 0; i < 3; i++) {
		float radius = SpiralRadius(uv, 0.09, float(i) * 0.18);
		c += smoothstep(radius - border, radius, dist) - smoothstep(radius, radius + border, dist);
	}

	float k = 1.0 - step(0.5, dist);
	FragColor = vec4(color * c, 1.0)* k;
} 


