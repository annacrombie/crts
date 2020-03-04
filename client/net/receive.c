#define _POSIX_C_SOURCE 201900L

#include <time.h>
#include <string.h>

#include "client/net/receive.h"
#include "shared/messaging/server_message.h"
#include "shared/serialize/server_message.h"
#include "shared/util/log.h"

#define BUFSIZE 4096
#define HEAP_SIZE 256

struct message_heap {
	struct {
		struct server_message e[HEAP_SIZE];
		size_t i;
	} sm;

	struct {
		struct sm_ent e[HEAP_SIZE];
		size_t i;
	} ent;

	struct {
		struct sm_chunk e[HEAP_SIZE];
		size_t i;
	} chunk;

	struct {
		struct sm_action e[HEAP_SIZE];
		size_t i;
	} action;

	struct {
		struct sm_rem_action e[HEAP_SIZE];
		size_t i;
	} rem_action;

	struct {
		struct sm_world_info e[HEAP_SIZE];
		size_t i;
	} world_info;
};

static struct message_heap *mh;

static void
wrap_inc(size_t *i)
{
	*i = *i >= HEAP_SIZE - 1 ? 0 : *i + 1;
}

static struct server_message *
unpack_message(struct message_heap *mh, const char *buf)
{
	size_t b;
	struct server_message *sm;

	sm = &mh->sm.e[mh->sm.i];
	wrap_inc(&mh->sm.i);

	b = unpack_sm(sm, buf);

	switch (sm->type) {
	case server_message_ent:
		b += unpack_sm_ent(&mh->ent.e[mh->ent.i], &buf[b]);
		sm->update = &mh->ent.e[mh->ent.i];

		wrap_inc(&mh->ent.i);
		break;
	case server_message_chunk:
		b += unpack_sm_chunk(&mh->chunk.e[mh->chunk.i], &buf[b]);
		sm->update = &mh->chunk.e[mh->chunk.i];

		wrap_inc(&mh->chunk.i);
		break;
	case server_message_action:
		b += unpack_sm_action(&mh->action.e[mh->action.i], &buf[b]);
		sm->update = &mh->action.e[mh->action.i];

		wrap_inc(&mh->action.i);
		break;
	case server_message_rem_action:
		b += unpack_sm_rem_action(&mh->rem_action.e[mh->rem_action.i], &buf[b]);
		sm->update = &mh->rem_action.e[mh->rem_action.i];

		wrap_inc(&mh->rem_action.i);
		break;
	case server_message_world_info:
		b += unpack_sm_world_info(&mh->world_info.e[mh->world_info.i], &buf[b]);
		sm->update = &mh->world_info.e[mh->world_info.i];

		wrap_inc(&mh->world_info.i);
		break;
	}

	return sm;
}

void
net_receive_init(void)
{
	mh = malloc(sizeof(struct message_heap));
	memset(mh, 0, sizeof(struct message_heap));
}

bool
net_receive(struct server_cx *s)
{
	bool recvd = false;
	char buf[BUFSIZE];
	int b;
	struct server_message *sm;

	union {
		struct sockaddr_in ia;
		struct sockaddr sa;
	} saddr;

	while ((b = recvfrom(s->sock, buf, BUFSIZE, 0, &saddr.sa, &socklen)) >= 1) {
		recvd = true;
		sm = unpack_message(mh, buf);

		queue_push(s->inbound, sm);
	}

	return recvd;
}
