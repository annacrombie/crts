#include <GLFW/glfw3.h>
#include <math.h>

#include "client/ui/opengl/globals.h"
#include "client/ui/opengl/input.h"
#include "shared/util/log.h"

enum modifier_types {
	mod_shift = 1 << 0,
};

static struct {
	uint8_t held[0xff];
	uint8_t mod;
} keyboard = { 0 };

static void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key < 0xff && key > 0) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			keyboard.held[key] = 1;
		} else {
			keyboard.held[key] = 0;
		}
	} else if (key == GLFW_KEY_LEFT_SHIFT) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			keyboard.mod |= mod_shift;
		} else {
			keyboard.mod &= ~mod_shift;
		}
	}
}

static void
mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static double lastx = 0, lasty = 0;

	cam.yaw   += (xpos - lastx) * 0.0026646259971648;
	cam.pitch += (ypos - lasty) * 0.0026646259971648;

	/*
	   if (cam.yaw > DEG_90) {
	        cam.yaw = DEG_90;
	   } else if (cam.yaw < -DEG_90) {
	        cam.yaw = -DEG_90;
	   }
	 */

	if (cam.pitch > DEG_90) {
		cam.pitch = DEG_90;
	} else if (cam.pitch < -DEG_90) {
		cam.pitch = -DEG_90;
	}

	lastx = xpos;
	lasty = ypos;

	cam.tgt.x = cos(cam.yaw) * cos(cam.pitch);
	cam.tgt.y = sin(cam.pitch);
	cam.tgt.z = sin(cam.yaw) * cos(cam.pitch);
	cam.changed = true;
}

void
handle_held_keys(void)
{
	size_t i;
	struct vec4 v1;

	float speed = 0.5;

	for (i = 0; i < 0xff; ++i) {
		if (!keyboard.held[i]) {
			continue;
		}

		if (keyboard.mod & mod_shift) {
			speed = 2.0;
		}

		switch (i) {
		case GLFW_KEY_S:
			v1 = cam.tgt;
			vec4_scale(&v1, speed);
			vec4_add(&cam.pos, &v1);
			cam.changed = true;
			break;
		case GLFW_KEY_W:
			v1 = cam.tgt;
			vec4_scale(&v1, speed);
			vec4_sub(&cam.pos, &v1);
			cam.changed = true;
			break;
		case GLFW_KEY_A:
			v1 = cam.tgt;
			vec4_cross(&v1, &cam.up);
			vec4_normalize(&v1);
			vec4_scale(&v1, speed);
			vec4_add(&cam.pos, &v1);
			cam.changed = true;
			break;
		case GLFW_KEY_D:
			v1 = cam.tgt;
			vec4_cross(&v1, &cam.up);
			vec4_normalize(&v1);
			vec4_scale(&v1, speed);
			vec4_sub(&cam.pos, &v1);
			cam.changed = true;
			break;
		default:
			break;
		}
	}
}

void
set_input_callbacks(struct GLFWwindow *window)
{
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	mouse_callback(window, 0, 0);
}
