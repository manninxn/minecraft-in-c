#version 410

layout (location = 0) in vec3 vertex_pos;

out vec2 uv;


void main()
{
    gl_Position = vec4(vertex_pos.x, vertex_pos.y, vertex_pos.z,1.0);
    uv = vec2(vertex_pos);
    
} 


//https://github.com/Hopson97/open-builder/blob/master/shaders/chunk_vertex.glsl