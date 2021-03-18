#ifndef CLIENT_UI_OPENGL_TEXT_H
#define CLIENT_UI_OPENGL_TEXT_H

#include <stddef.h>

#include "shared/math/linalg.h"
#include "shared/opengl/window.h"
#include "shared/types/geom.h"

bool render_text_setup(void);
void render_text_add(float *x, float *y, const vec4 clr, const char *str);
void render_text_clear(void);
void render_text_commit(void);
void render_text(struct gl_win *win, mat4 proj);
#endif
