#include "libft.h"
#include "server.h"
#include "websocket.h"

static void			fs_waitroom(wslay_event_context_ptr ctx,
					t_env *env, int idx)
{
	unsigned char *buftmp;

	if (env)
	{
		buftmp = pipe_com_read(env->ptoc_pipe[idx][0]);

// 		fprintf(stdout, "[%s] [%d], BUFTMP = [%s]\n", __FILE__, __LINE__, buftmp);
		if (buftmp)
		{
			if (check_con_list(env, idx, buftmp))
			{
				cvtInt(env->msg, idx);
				strcat(env->msg, "/");
				strcat(env->msg, env->lan_msg[idx]);
				send_text_msg(ctx, env->msg);
			}
			else if (check_con_challenger(env, idx, buftmp))
			{
				cvtInt(env->msg, idx);
				strcat(env->msg, "/");
				strcat(env->msg, env->lan_msg[idx]);
				send_text_msg(ctx, env->msg);
			}
			else if (check_ws_ready(env, idx, buftmp))
			{
				fprintf(stdout, "[%s] [%d], env->msg = [%s]\n", __FILE__, __LINE__, env->msg);//_DEBUG_//
				strcpy(env->msg, env->lan_msg[idx]);
				send_text_msg(ctx, env->msg);
			}
			else if (check_ws_allready(env, idx, buftmp))
			{
				cvtInt(env->msg, idx);
				strcat(env->msg, "/");
				strcat(env->msg, env->lan_msg[idx]);
				send_text_msg(ctx, env->msg);
				env->state = START;
				fprintf(stdout, "[%s] [%d], env->msg = [%s]\n", __FILE__, __LINE__, env->msg);
			}

			free(buftmp);
		}
	}
}

static void			fs_start(wslay_event_context_ptr ctx,
					t_env *env, int idx)
{
	unsigned char *buftmp;

	if (env)
	{
		buftmp = pipe_com_read(env->ptoc_pipe[idx][0]);
		if (buftmp)
		{
			if (check_ws_start(env, idx, buftmp))
			{
				strcpy(env->msg, env->lan_msg[idx]);
				send_text_msg(ctx, env->msg);
				fprintf(stdout, "[%s] [%d], env->msg = [%s]\n", __FILE__, __LINE__, env->msg);
			}
			else if (check_ws_guess(env, idx, buftmp))
			{
				strcpy(env->msg, env->lan_msg[idx]);
				send_text_msg(ctx, env->msg);
				fprintf(stdout, "[%s] [%d], env->msg = [%s]\n", __FILE__, __LINE__, env->msg);
			}
			else if (check_ws_winner(env, idx, buftmp))
			{
				cvtInt(env->msg, idx);
				strcat(env->msg, "/");
				strcat(env->msg, env->lan_msg[idx]);
				send_text_msg(ctx, env->msg);
				env->state = START;
				fprintf(stdout, "[%s] [%d], env->msg = [%s]\n", __FILE__, __LINE__, env->msg);
			}
			free(buftmp);
		}
	}
}


static void			init_fs_tab(fstate_c *fs_tab)
{
	if (fs_tab)
	{
		fs_tab[WAITROOM] = fs_waitroom;
		fs_tab[START] = fs_start;
	}
}

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

	struct Session 					session = {sock, env, idx};
	struct pollfd					event;
	socklen_t						len;
	struct sockaddr_in				addr;
	fstate_c						fs_tab[END];
	int								out;

	init_fs_tab(fs_tab);
	len = sizeof(struct sockaddr_in);
	(void)out;
	out = 0;
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
		if (env->isfull)
			send_text_msg(ctx, "-1/ws/full");
		else
		{
			//Call env->state corresponding function
			fs_tab[env->state](ctx, env, idx);
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
