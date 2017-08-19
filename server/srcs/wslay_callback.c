#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <wslay/wslay.h>
#include "websocket.h"
#include "server.h"

/*
 * Makes file descriptor *fd* non-blocking mode.
 * This function returns 0, or returns -1.
 */
int 				make_non_block(int fd)
{
	int				flags;
	int				r;

	if((flags = fcntl(fd, F_GETFL, 0)) == -1)
	{
		perror("fcntl");
		return -1;
	}
	if((r = fcntl(fd, F_SETFL, flags | O_NONBLOCK)) == -1)
	{
		perror("fcntl");
		return -1;
	}
	return 0;
}

ssize_t send_callback(wslay_event_context_ptr ctx,
		const uint8_t *data, size_t len, int flags,
		void *user_data)
{
	struct Session *session = (struct Session*)user_data;
	ssize_t r;
	int sflags = 0;
#ifdef MSG_MORE
	if(flags & WSLAY_MSG_MORE) {
		sflags |= MSG_MORE;
	}
#endif // MSG_MORE
	while((r = send(session->fd, data, len, sflags)) == -1 && errno == EINTR);
	if(r == -1) {
		if(errno == EAGAIN || errno == EWOULDBLOCK) {
			wslay_event_set_error(ctx, WSLAY_ERR_WOULDBLOCK);
		} else {
			wslay_event_set_error(ctx, WSLAY_ERR_CALLBACK_FAILURE);
		}
	}
	return r;
}

ssize_t recv_callback(wslay_event_context_ptr ctx, uint8_t *buf, size_t len,
		int flags, void *user_data)
{
	(void)flags;
	struct Session *session = (struct Session*)user_data;
	ssize_t r;
	while((r = recv(session->fd, buf, len, 0)) == -1 && errno == EINTR);
	if(r == -1)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK) {
			wslay_event_set_error(ctx, WSLAY_ERR_WOULDBLOCK);
		}
		else
		{
			wslay_event_set_error(ctx, WSLAY_ERR_CALLBACK_FAILURE);
		}
	}
	else if(r == 0)
	{
		/* Unexpected EOF is also treated as an error */
		wslay_event_set_error(ctx, WSLAY_ERR_CALLBACK_FAILURE);
		r = -1;
	}
	//   else
	//   {
	// 	  fprintf(stdout, buf);
	//   }
	return r;
}

void			on_msg_recv_callback(wslay_event_context_ptr ctx,
		const struct wslay_event_on_msg_recv_arg *arg,
		void *user_data)
{
	(void)user_data;
	/* Echo back non-control message */
	if(!wslay_is_ctrl_frame(arg->opcode))
	{
		(void)ctx;
// 		struct wslay_event_msg msgarg = {
// 			arg->opcode,
// 			arg->msg,
// 			arg->msg_length
// 		};
// 		msgarg.msg = (const uint8_t*)"je suis con";
// 		msgarg.msg_length = strlen((const char*)msgarg.msg);
// 		wslay_event_queue_msg(ctx, &msgarg);
	}
}
