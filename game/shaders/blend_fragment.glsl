#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D anim;

uniform int width;
uniform int height;

uniform int index;


uniform float time;

#define PI 3.14159265358979323846


vec4 Maxx2(float tt, vec4 c1, vec4 c2) {
	float t = tt ;
	// float t = sqrt(tt);
	return (1.0 - t) * c1 + c2 * t;
	//return c1;
}

vec4 Maxx(float tt, vec4 c1, vec4 c2) {
	float t = tt;
	vec3 rgb = (1.0 - t) * c1.rgb + c2.rgb * t;
	return vec4(rgb, max(c1.a, c2.a));
}

vec2 SpriteSheet(int i, vec2 tCoord) {
	vec2 coord = tCoord;
	coord.x /= float(width);
	coord.x += float(i) / float(width);

	coord.y = 1.0 - coord.y;

	coord.y /= float(height);
	coord.y += float(i / width) * coord.y;
	
	return coord;
}

void main() {

	int index2 = index - 1;
	if (index2 < 0) index2 = 0;
	
	vec2 coord = SpriteSheet(index, TexCoord);
	vec2 coord2 = SpriteSheet(index2, TexCoord);

	// float t = sin(time * PI) * 0.5 + 0.5 + float(index2 + index) * 0.0001;
	float t = fract(time * 2);

	//t = clamp(t, 0.2, 1.0);

	vec4 col = texture(anim, coord);
	vec4 col2 = texture(anim, coord2);

	if (col.a < 0.4) col.rgb = vec3(0.0);
	if (col2.a < 0.4) col2.rgb = vec3(0.0);


	FragColor = Maxx2(t, col, col2);
} 


