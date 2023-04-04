#version 330 core
in float shade;
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;
uniform vec3 camera_pos;

float max_depth = 100.0;

void main(){
	float depth = gl_FragCoord.z / gl_FragCoord.w;
	FragColor = texture(texture1, TexCoord) * shade;

} 