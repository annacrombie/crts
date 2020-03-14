#ifndef SHARED_NET_DEFS_H
#define SHARED_NET_DEFS_H

#include <arpa/inet.h>
#include <stdint.h>
#include <sys/socket.h>

#define BUFSIZE 4096
#define MAX_CXS 32
#define MSG_ID_LIM 512lu
#define FRAME_LEN MSG_ID_LIM
#define MSG_HDR_LEN sizeof(struct msg_hdr)
#define MSG_RESEND_AFTER 4
#define MSG_DESTROY_AFTER 2

extern socklen_t socklen;

typedef uint16_t msg_seq_t;
typedef uint32_t msg_ack_t;
typedef uint32_t cx_bits_t;

enum msg_flags {
	msgf_forget = 1 << 0,
	msgf_ack = 1 << 1
};

struct msg_hdr {
	uint16_t flags;
	msg_seq_t seq;
};

#endif