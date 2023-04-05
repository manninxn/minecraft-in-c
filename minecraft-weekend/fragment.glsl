#version 330 core
in float shade;
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;
uniform vec3 camera_pos;
uniform ivec2 viewport_size;
float max_depth = 100.0;

vec3 color1 = vec3(.0,.7,1);
vec3 color0 = vec3(0.8,0.8,0.8);

uniform vec3 look_dir;

const vec3 up = vec3(0.0,1.0,0.0);


void main(){
	float depth = gl_FragCoord.z / gl_FragCoord.w;
	float look_y = look_dir.y;
	vec4 fogColor = vec4(mix(color0, color1, clamp(look_y + (gl_FragCoord.y / viewport_size.y + (look_y * 0.25)), 0.0, 1.0)), 1.0);
	FragColor = mix(texture(texture1, TexCoord) * shade, fogColor, clamp(depth / max_depth, 0.0, 1.0));

} 