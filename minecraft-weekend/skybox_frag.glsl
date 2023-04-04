#version 330 core
in vec2 uv;
out vec4 FragColor;

vec3 color1 = vec3(.0,.7,1);
vec3 color0 = vec3(0.8,0.8,0.8);

uniform vec3 look_dir;

const vec3 up = vec3(0.0,1.0,0.0);

void main(){
	float look_y = look_dir.y;
	FragColor =  vec4(mix(color0, color1, look_y + (uv.y + (look_y * 0.25))), 1.0);

}