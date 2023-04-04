#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include "gfx_common.h"
#include <cglm/types-struct.h>


struct Camera {
	vec3s position;
	vec3s look_direction;
	float fov;
	float near_clip, far_clip;
	float last_delta_x, last_delta_y;
	float pitch, yaw;
	float speed;
	ivec2s viewport_size;
};

struct Camera camera_new(vec3s position, vec3s direction, float fov, float near_clip, float far_clip);

struct ViewProjection camera_gew_view_projection(struct Camera cam);

void camera_handle_mouse_movement(struct Camera* cam, float delta_x, float delta_y);

void camera_handle_input(struct Camera* cam);




#endif