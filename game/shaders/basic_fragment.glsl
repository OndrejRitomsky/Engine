#version 330 core


in vec2 TexCoord;

out vec4 FragColor;

//uniform vec3 objectColor;

uniform sampler2D diffuseTexture;

uniform float time;

#define PI 3.14159265358979323846

vec2 tile(vec2 st, float _zoom){
    st *= _zoom;
    return fract(st);
}

float box(vec2 _st, vec2 _size, float _smoothEdges){
    _size = vec2(0.5)-_size*0.5;
    vec2 aa = vec2(_smoothEdges*0.5);
    vec2 uv = smoothstep(_size,_size+aa,_st);
    uv *= smoothstep(_size,_size+aa,vec2(1.0)-_st);
    return uv.x*uv.y;
}

vec2 rotate2D(vec2 _st, float _angle){
    _st -= 0.5;
    _st =  mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle)) * _st;
    _st += 0.5;
    return _st;
}

void main() {
	//vec2 reso = vec2(1024.0 + time / 10000000.f ,726.0);
	//vec2 reso = vec2(1024.0 + time / 10000000.f ,726.0);

	//vec2 pos = gl_FragCoord.xy / reso.xy;
	//pos = tile(pos, 4);
	//pos = rotate2D(pos, PI*fract(time));
	
	//pos = rotate2D(pos, PI*0.25);

	//vec4 color = vec4(vec3(box(pos,vec2(0.7),0.01), 0, 0), 1.0);
	vec4 color = vec4(sin(time) * 0.5 + 0.5, 0, 0, 1.0);
	vec2 cc = vec2(TexCoord.x, 1 - TexCoord.y);
	
	float dist = length(TexCoord - vec2(0.5));

	FragColor = color * 0.001 + texture(diffuseTexture, cc);
} 


