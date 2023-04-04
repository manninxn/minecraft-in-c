#version 330 core
layout (location = 0) in uint vertex_data;
layout (location = 1) in uint lighting_data;
out float shade;
out vec2 TexCoord;
  
uniform ivec2 atlas_params;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform ivec3 chunk_pos;


vec2 texCoords[4] = vec2[4] (

    vec2(0.0,0.0),
    vec2(1.0,0.0),
    vec2(1.0,1.0),
    vec2(0.0,1.0)
);

float shade_strengths[6] = float[6] (
  0.8,
  0.7,
  0.8,
  0.7,
  0.9,
  0.3
);


const int chunk_size = 16;


void main()
{
    float x = float(vertex_data & 0x3Fu);
    float y = float((vertex_data & 0xFC0u) >> 6u);
    float z = float((vertex_data & 0x3F000u) >> 12u);
    x += float(chunk_pos.x * chunk_size);
    y += float(chunk_pos.y * chunk_size);
    z += float(chunk_pos.z * chunk_size);
    gl_Position = projection * view * vec4(x, y, z, 1.0);

    //Texture coords
    uint index = (vertex_data & 0xc0000u) >> 18u;
    float sprite_x = float((vertex_data & 0xf00000u) >> 20u);
    float sprite_y = float((vertex_data & 0xf000000u) >> 24u);

    uint normal_index = (vertex_data & 0x70000000u) >> 28u;
    

    uint ao_state = lighting_data;
    shade = clamp(shade_strengths[normal_index] - float(ao_state) * 0.25, 0, 1);

    

    float sprite_size = float(atlas_params.x) / float(atlas_params.y);

    TexCoord = texCoords[index] * sprite_size + vec2(sprite_x * sprite_size, sprite_y * sprite_size);
} 


//https://github.com/Hopson97/open-builder/blob/master/shaders/chunk_vertex.glsl