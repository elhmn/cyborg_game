#include "libft.h"
#include "server.h"
#include "websocket.h"

int					communication_handler(t_env *env, int idx, int sock)
{
	wslay_event_context_ptr 		ctx;
	struct wslay_event_callbacks 	callbacks = {
													recv_callback,
													send_callback,
													NULL,
													NULL,
													NULL,
													NULL,
													on_msg_recv_callback
												};
	struct Session 					session = {sock};
	struct pollfd					event;
	socklen_t						len;
	struct sockaddr_in				addr;

	len = sizeof(struct sockaddr_in);
	if (getpeername(sock, (struct sockaddr*)&addr, &len) < 0)
	{
		perror("getpeername");
		return (-1);
	}
 	fprintf(stdout, "Peer : ");
	put_addr((struct sockaddr*)&addr, len);
	if (http_handshake(sock) < 0)
	{
		perror("http_handshake");
		return (-1);
	}
	if (make_non_block(sock) < 0)
	{
		perror("make_non_block");
		return (-1);
	}
	memset(&event, 0, sizeof(struct pollfd));
	event.fd = sock;
	event.events = POLLIN;
	wslay_event_context_server_init(&ctx, &callbacks, &session);

	while(wslay_event_want_read(ctx) || wslay_event_want_write(ctx))
	{
		if (check_con_list(env, idx))
		{
			fprintf(stdout, "SENT======\n");//_DEBUG_//
			struct wslay_event_msg msgarg;

			msgarg.opcode = WSLAY_TEXT_FRAME;
			cvtInt(env->msg, idx);
			strcat(env->msg, "/");
			strcat(env->msg, env->lan_msg);
			msgarg.msg = (const uint8_t*)env->msg;
			msgarg.msg_length = strlen((const char*)msgarg.msg);
			wslay_event_queue_msg(ctx, &msgarg);
		}

		if(poll(&event, 1, -1) == -1)
		{
			perror("poll");
			return (-1);
		}
		if(((event.revents & POLLIN) && wslay_event_recv(ctx) != 0)
			|| ((event.revents & POLLOUT) && wslay_event_send(ctx) != 0)
			|| (event.revents & (POLLERR | POLLHUP | POLLNVAL)))
		{
			(void)env;
			(void)idx;

			/*
			 * If either wslay_event_recv() or wslay_event_send() return
			 * non-zero value, it means serious error which prevents wslay
			 * library from processing further data, so WebSocket connection
			 * must be closed.
			 */
			perror("wslay_event_recv or send Fatal");
			return (-1);
		}
		event.events = 0;
		if(wslay_event_want_read(ctx))
		{
			event.events |= POLLIN;
		}
		if(wslay_event_want_write(ctx))
		{
			event.events |= POLLOUT;
		}
	}
	close(sock);
	return (0);
}
