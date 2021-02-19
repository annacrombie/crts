#ifndef SHARED_MSGR_TRANSPORT_RUDP_H
#define SHARED_MSGR_TRANSPORT_RUDP_H

#include "shared/msgr/msgr.h"
#include "shared/msgr/transport/rudp/cx_pool.h"
#include "shared/platform/sockets/common.h"
#include "shared/types/sack.h"

struct msg_sack_hdr {
	msg_addr_t dest;
	msg_seq_t msg_id;
	uint32_t times_sent;
	uint8_t send_cooldown;
};

struct msgr_transport_rudp_ctx {
	struct sack msg_sk_send, msg_sk_recv;
	struct cx_pool pool;
	const struct sock_impl *si;
	sock_t sock;
	uint16_t seq, msg_id;
};

void rudp_connect(struct msgr *msgr, struct sock_addr *addr);
bool msgr_transport_init_rudp(struct msgr_transport_rudp_ctx *ctx,
	struct msgr *msgr, const struct sock_impl *impl,
	struct sock_addr *bind_addr);
#endif
