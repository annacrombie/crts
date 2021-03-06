#ifndef CLIENT_UI_OPENGL_GLOBALS_H
#define CLIENT_UI_OPENGL_GLOBALS_H

#include <stdint.h>

#include "shared/math/linalg.h"

#define DEG_90 1.57f
#define FOV 0.47f
#define NEAR 0.1f
#define FAR 1000.0f
#define CAM_PITCH 1.143190660056286

extern struct camera cam;
extern struct camera sun;

#define CHUNK_INDICES_LEN (512 * 3)
extern const uint32_t chunk_indices[CHUNK_INDICES_LEN];
#endif
