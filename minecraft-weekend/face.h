#ifndef FACE_H
#define FACE_H


#define NORTH_VERTS(x, y, z, sprite_x, sprite_y, ao_a, ao_b, ao_c, ao_d) {														\
	((x + 1) | ((y + 1) << 6u) | ((z + 1) << 12u)) | (2 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (0 << 28u), ao_a,											\
	((x + 1) | ((y + 0) << 6u) | ((z + 1) << 12u)) | (1 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (0 << 28u), ao_b,											\
	((x + 0) | ((y + 0) << 6u) | ((z + 1) << 12u)) | (0 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (0 << 28u), ao_c,											\
	((x + 0) | ((y + 1) << 6u) | ((z + 1) << 12u)) | (3 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (0 << 28u), ao_d,									\
}														  
														  
#define SOUTH_VERTS(x, y, z, sprite_x, sprite_y, ao_a, ao_b, ao_c, ao_d) {													\
	((x + 1) | ((y + 1) << 6u) | ((z + 0) << 12u)) | (2 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (1 << 28u), ao_a,												\
	((x + 1) | ((y + 0) << 6u) | ((z + 0) << 12u)) | (1 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (1 << 28u), ao_b,												\
	((x + 0) | ((y + 0) << 6u) | ((z + 0) << 12u)) | (0 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (1 << 28u), ao_c,												\
	((x + 0) | ((y + 1) << 6u) | ((z + 0) << 12u)) | (3 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (1 << 28u), ao_d,												\
}														  
														  
#define EAST_VERTS(x, y, z, sprite_x, sprite_y, ao_a, ao_b, ao_c, ao_d) {														\
	((x + 1) | ((y + 1) << 6u) | ((z + 0) << 12u)) | (2 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (2 << 28u), ao_a,												\
	((x + 1) | ((y + 0) << 6u) | ((z + 0) << 12u)) | (1 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (2 << 28u), ao_b,												\
	((x + 1) | ((y + 0) << 6u) | ((z + 1) << 12u)) | (0 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (2 << 28u), ao_c,												\
	((x + 1) | ((y + 1) << 6u) | ((z + 1) << 12u)) | (3 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (2 << 28u), ao_d,												\
}														   
														   
#define WEST_VERTS(x, y, z, sprite_x, sprite_y, ao_a, ao_b, ao_c, ao_d) {													\
	((x + 0) | ((y + 1) << 6u) | ((z + 0) << 12u)) | (2 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (3 << 28u), ao_a,											\
	((x + 0) | ((y + 0) << 6u) | ((z + 0) << 12u)) | (1 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (3 << 28u), ao_b,											\
	((x + 0) | ((y + 0) << 6u) | ((z + 1) << 12u)) | (0 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (3 << 28u), ao_c,											\
	((x + 0) | ((y + 1) << 6u) | ((z + 1) << 12u)) | (3 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (3 << 28u), ao_d,											\
}														   
														   
#define BOTTOM_VERTS(x, y, z, sprite_x, sprite_y, ao_a, ao_b, ao_c, ao_d) {													\
	((x + 1) | ((y + 0) << 6u) | ((z + 1) << 12u)) | (2 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (4 << 28u), ao_a,												\
	((x + 1) | ((y + 0) << 6u) | ((z + 0) << 12u)) | (1 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (4 << 28u), ao_b,												\
	((x + 0) | ((y + 0) << 6u) | ((z + 0) << 12u)) | (0 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (4 << 28u), ao_c,												\
	((x + 0) | ((y + 0) << 6u) | ((z + 1) << 12u)) | (3 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (4 << 28u), ao_d,												\
}														  
														  
#define TOP_VERTS(x, y, z, sprite_x, sprite_y, ao_a, ao_b, ao_c, ao_d) {													\
	((x + 1) | ((y + 1) << 6u) | ((z + 1) << 12u)) | (2 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (5 << 28u), ao_a,											\
	((x + 1) | ((y + 1) << 6u) | ((z + 0) << 12u)) | (1 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (5 << 28u), ao_b,											\
	((x + 0) | ((y + 1) << 6u) | ((z + 0) << 12u)) | (0 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (5 << 28u), ao_c,											\
	((x + 0) | ((y + 1) << 6u) | ((z + 1) << 12u)) | (3 << 18u) | (sprite_x << 20u) | (sprite_y << 24u) | (5 << 28u), ao_d,											\
}

#define INDICES_INWARDS(offset) {  														 \
	offset + 3, offset + 1, offset,														 \
	offset + 3, offset + 2, offset + 1,													 \
}

#define INDICES_OUTWARDS(offset) {														 \
	offset, offset + 1, offset + 3,														 \
	offset + 1, offset + 2, offset + 3													 \
}



#endif