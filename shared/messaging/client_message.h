#ifndef __CLIENT_MESSAGE_H
#define __CLIENT_MESSAGE_H
#include "math/geom.h"
#include "sim/action.h"

enum client_message_type {
	client_message_poke,
	client_message_action,
	client_message_chunk_req
};

struct client_message {
	enum client_message_type type;
	void *update;
};

struct cm_poke {};

struct cm_chunk_req {
	struct point pos;
};

struct cm_action {
	enum action_type type;
	struct circle range;
};

struct client_message *cm_create(enum client_message_type t, void *e);
void cm_destroy(struct client_message *ud);
#endif