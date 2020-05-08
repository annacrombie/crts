#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "client/ui/opengl/color_cfg.h"
#include "client/ui/opengl/globals.h"
#include "client/ui/opengl/input.h"
#include "client/ui/opengl/solids.h"
#include "client/ui/opengl/ui.h"
#include "client/ui/opengl/winutil.h"
#include "shared/math/linalg.h"
#include "shared/sim/ent.h"
#include "shared/types/darr.h"
#include "shared/types/hash.h"
#include "shared/util/log.h"

enum render_categories {
	rcat_chunk = 0,
	rcat_ents = 1,
};

/* Needed for resize_callback */
static struct opengl_ui_ctx *global_ctx;

static void
resize_callback(struct GLFWwindow *win, int width, int height)
{
	mat4 mproj;
	glViewport(0, 0, width, height);

	gen_perspective_mat4(0.47, (float)width / (float)height, 0.1, 1000.0, mproj);
	glUniformMatrix4fv(global_ctx->chunks.uni.proj, 1, GL_TRUE, (float *)mproj);
}

static bool
setup_program_chunks(struct opengl_ui_ctx *ctx)
{
	struct shader_src src[] = {
		{ "client/ui/opengl/shaders/shader.vert", GL_VERTEX_SHADER   },
		{ "client/ui/opengl/shaders/shader.frag", GL_FRAGMENT_SHADER },
		{ "\0" }
	};

	ctx->echash = hash_init(2048, 1, sizeof(struct point));

	if (!link_shaders(src, &ctx->chunks.id)) {
		return false;
	}

	ctx->chunks.uni.view      = glGetUniformLocation(ctx->chunks.id, "view");
	ctx->chunks.uni.proj      = glGetUniformLocation(ctx->chunks.id, "proj");
	ctx->chunks.uni.clr       = glGetUniformLocation(ctx->chunks.id, "tile_color");
	ctx->chunks.uni.positions = glGetUniformLocation(ctx->chunks.id, "positions");
	ctx->chunks.uni.types     = glGetUniformLocation(ctx->chunks.id, "types");
	ctx->chunks.uni.cat       = glGetUniformLocation(ctx->chunks.id, "cat");
	ctx->chunks.uni.bases     = glGetUniformLocation(ctx->chunks.id, "bases");
	ctx->chunks.uni.view_pos  = glGetUniformLocation(ctx->chunks.id, "view_pos");

	glGenVertexArrays(1, &ctx->chunks.vao);
	glGenBuffers(1, &ctx->chunks.vbo);

	glBindVertexArray(ctx->chunks.vao);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->chunks.vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * solid_cube.len,
		solid_cube.verts, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void *)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return true;
}

struct opengl_ui_ctx *
opengl_ui_init(char *graphics_path)
{
	struct opengl_ui_ctx *ctx = calloc(1, sizeof(struct opengl_ui_ctx));
	global_ctx = ctx;

	if (!(ctx->window = init_window())) {
		goto free_exit;
	} else if (!setup_program_chunks(ctx)) {
		goto free_exit;
	} else if (!color_cfg(graphics_path, ctx)) {
		goto free_exit;
	}

	/* Set callbacks */
	set_input_callbacks(ctx->window);
	glfwSetFramebufferSizeCallback(ctx->window, resize_callback);
	glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glUseProgram(ctx->chunks.id);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return ctx;
free_exit:
	opengl_ui_deinit(ctx);
	return NULL;
}

static void
render_chunks(struct chunks *cnks, struct opengl_ui_ctx *ctx)
{
	struct chunk *cmem = darr_raw_memory(hdarr_darr(cnks->hd));
	size_t ci, len = hdarr_len(cnks->hd);

	int ipos[3] = { 0 };

	uint32_t cat = rcat_chunk;
	glUniform1uiv(ctx->chunks.uni.cat, 1, &cat);

	for (ci = 0; ci < len; ++ci) {
		ipos[0] = cmem[ci].pos.x;
		ipos[1] = cmem[ci].pos.y;

		glUniform3iv(ctx->chunks.uni.positions, 1, ipos);
		glUniform1uiv(ctx->chunks.uni.types, 256, (uint32_t *)&cmem[ci].tiles);

		/* draw on extra for the chunk's base */
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 256 + 1);
	}
}

static void
render_ents(struct hdarr *ents, struct hdarr *cnks, struct opengl_ui_ctx *ctx)
{
	struct ent *emem = darr_raw_memory(hdarr_darr(ents));
	size_t i, j, len = hdarr_len(ents);

	hash_clear(ctx->echash);

	uint32_t cat = rcat_ents;
	glUniform1uiv(ctx->chunks.uni.cat, 1, &cat);

	/*
	   struct chunk *ck;
	   struct point p;
	 */

	int32_t positions[256 * 3] = { 0 };
	uint32_t types[256] = { 0 };
	//uint32_t bases[256];
	const size_t *st;

	for (i = 0, j = 0; i < len; ++i, ++j) {
		if (i >= 256) {
			glUniform1uiv(ctx->chunks.uni.types, 256, types);
			glUniform3iv(ctx->chunks.uni.positions, 256, positions);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 256);
			j = 0;
		}

		positions[(j * 3) + 0] = emem[i].pos.x;
		positions[(j * 3) + 1] = emem[i].pos.y;

		if ((st = hash_get(ctx->echash, &emem[i].pos))) {
			positions[(j * 3) + 2] = *st + 1;
			hash_set(ctx->echash, &emem[i].pos, *st + 1);
		} else {
			positions[(j * 3) + 2] = 0;
			hash_set(ctx->echash, &emem[i].pos, 0);
		}

		types[j] = emem[i].type;

		/*
		   p = nearest_chunk(&emem[i].pos);
		   if ((ck = hdarr_get(cnks, &p))) {
		        p = point_sub(&emem[i].pos, &p);
		        bases[i] = ck->tiles[p.x][p.y];
		   } else {
		        bases[i] = tile_deep_water;
		   }
		 */
	}
	glUniform1uiv(ctx->chunks.uni.types, 256, types);
	glUniform3iv(ctx->chunks.uni.positions, 256, positions);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, len % 256);
}

void
opengl_ui_render(struct opengl_ui_ctx *ctx, struct hiface *hf)
{
	if (cam.changed) {
		mat4 mview;

		cam.tgt[0] = cos(cam.yaw) * cos(cam.pitch);
		cam.tgt[1] = sin(cam.pitch);
		cam.tgt[2] = sin(cam.yaw) * cos(cam.pitch);

		gen_look_at(&cam, mview);
		glUniformMatrix4fv(ctx->chunks.uni.view, 1, GL_TRUE, (float *)mview);
		glUniform3fv(ctx->chunks.uni.view_pos, 1, cam.pos);
		cam.changed = false;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_chunks(hf->sim->w->chunks, ctx);

	render_ents(hf->sim->w->ents, hf->sim->w->chunks->hd, ctx);

	glfwSwapBuffers(ctx->window);
}

void
opengl_ui_handle_input(struct opengl_ui_ctx *ctx, struct keymap **km,
	struct hiface *hf)
{
	glfwPollEvents();
	handle_held_keys(hf);

	if (glfwWindowShouldClose(ctx->window)) {
		hf->sim->run = false;
	} else if (!hf->sim->run) {
		glfwSetWindowShouldClose(ctx->window, 1);
	}
}

struct rectangle
opengl_ui_viewport(struct opengl_ui_ctx *nc)
{
	struct rectangle r = { { 0, 0 }, 128, 128 };

	return r;
}

void
opengl_ui_deinit(struct opengl_ui_ctx *ctx)
{
	hash_destroy(ctx->echash);
	free(ctx);
	glfwTerminate();
}