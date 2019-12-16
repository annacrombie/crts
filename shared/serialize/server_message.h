#ifndef __SERIALIZE_SERVER_MESSAGE_H
#define __SERIALIZE_SERVER_MESSAGE_H
#include "messaging/server_message.h"

size_t pack_sm(const struct server_message *ud, char *buf);
size_t unpack_sm(struct server_message *ud, const char *buf);

size_t pack_sm_ent(const struct sm_ent *eu, char *buf);
size_t unpack_sm_ent(struct sm_ent *eu, const char *buf);

size_t pack_sm_chunk(const struct sm_chunk *eu, char *buf);
size_t unpack_sm_chunk(struct sm_chunk *eu, const char *buf);
#endif