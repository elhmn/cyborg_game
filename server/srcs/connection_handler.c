#include "libft.h"
#include "server.h"
#include "websocket.h"

static int			all_ready(t_env e)
{
	int		i;

	for (i = 0; i < MAXPLAYER; i++)
	{
		if (e.ready[i] == 0)
			return (0);
	}
	return (1);
}

static void			fs_waitroom(t_env *env)
{
	char **tab;

	tab = NULL;
// 	fprintf(stdout, "fs_waitroom [%s], [%d]\n", __FILE__, __LINE__);//_DEBUG_//
	if (env)
	{
		tab = ft_strsplit((char*)env->rcv_msg[env->rcv_idx], '/');
		if (tab)
		{
// 			show_tab(tab);
			//check what ever you want
			if (update_ready_tab(env, tab))
			{
				send_ws_ready(env);
				if (all_ready(*env))
				{
					usleep(10);
					send_ws_allready(env);
					fprintf(stdout, "The game can start !!\n");
				}
			}
			free_tab(&tab);
		}
	}
}

static void			init_fs_tab(fstate_p *fs_tab)
{
	if (fs_tab)
	{
		fs_tab[WAITROOM] = fs_waitroom;
	}
}

static void			get_ip(char *buf, int sock)
{
	socklen_t						len;
	struct sockaddr_in				addr;
	char							hostbuf[NI_MAXHOST];
	char							servbuf[NI_MAXSERV];
	int								s;

	if (buf)
	{
		len = sizeof(struct sockaddr_in);
		if (getpeername(sock, (struct sockaddr*)&addr, &len) < 0)
		{
			perror("getpeername");
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "getip Peer : ");//_DEBUG_//

		//get ip info
		if ((s = getnameinfo((struct sockaddr*)&addr, len, hostbuf, NI_MAXHOST,
							servbuf, NI_MAXSERV, NI_NUMERICHOST
							| NI_NUMERICSERV)) != 0)
		{
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "ip = [%s] , port = [%s] \n",
				hostbuf, servbuf);
		strcpy(buf, hostbuf);
		fprintf(stdout, "ip = [%s]\n", buf);
	}
}

//check whether the server must be closed or not
static int			quit_server(void)
{
	return (0);
}

static int			choose_sock_idx(t_env e, int idx)
{
	int		i;

	if (e.com_tab[idx].sock == -1)
		return (idx);
	else
	{
		for (i = 0; i < MAXPLAYER; i++)
		{
			if (e.com_tab[i].sock == -1)
				return (i);
		}
	}
	return (idx);
}

static int			has_challenger(t_env e)
{
	int		i;
	int		n;

	n = 0;
	for (i = 0; i < MAXPLAYER; i++)
	{
		if (e.com_tab[i].sock != -1)
		{
			if (++n > 1)
				return (1);
		}
	}
	return (0);
}


static int			isfull(t_env *e)
{
	int		i;

	for (i = 0; i < MAXPLAYER; i++)
	{
		if (e->com_tab[i].sock == -1)
		{
			e->isfull = 0;
			return (0);
		}
	}
	e->isfull = 1;
	return (1);
}

/*
** Here I set env as global cus I wanted to use signal
** to handle basic IPC communication instead of sigaction
*/

t_env				g_env;

static void			parent_signal_handler(int sig)
{
	fstate_p		fs_tab[END];

	init_fs_tab(fs_tab);
	if (sig == SIGRT_CLOSE)
	{
// 		fprintf(stdout, "je suis con");//_DEBUG_//
		//father pipe com event handler
		if (check_deconnection(&g_env))
		{
// 			fprintf(stdout, "je me suis deconnecte !\n");
			// Update connection list
			update_con_list(&g_env);

			// Send connection list to children
			send_con_list(&g_env);
		}
		//check if has challenger
		if (has_challenger(g_env))
			send_con_challenger(&g_env, 1);
		else
			send_con_challenger(&g_env, 0);
	}
	else if (sig == SIGRT_RCV)
	{
		if (check_rcv_msg(&g_env))
		{
			fprintf(stdout, "-----Parent received message [%s]\n", g_env.rcv_msg[g_env.rcv_idx]);//_DEBUG_//
			//Call env->state corresponding function
			fs_tab[g_env.state](&g_env);
			fprintf(stdout, "------Parent received message [%s]\n", g_env.rcv_msg[g_env.rcv_idx]);//_DEBUG_//
		}
	}
}

int					connection_handler(int sock)
{
	int					sock_com;
	int					full;
	int					idx;

	socklen_t			len;
	struct sockaddr_in	addr;

	full = 0;
	idx = 0;
	len = sizeof(struct sockaddr_in);
	init_env(&g_env);

	//inform kernel that we want to receive connection on that socket
	if (listen(sock, 5) < 0)
	{
		perror("listen");
		return (-1);
	}
	while (!quit_server())
	{
		put_comtab(g_env.com_tab, MAXPLAYER);//_DEBUG_//
		memset(&addr, 0, len);
		if ((sock_com = accept(sock, (struct sockaddr*)&addr, &len)) < 0)
		{
			perror("accept");
			return (-1);
		}
		//if no more place in the room
		if (isfull(&g_env))
		{
			switch (fork())
			{
				//child
				case 0:
				close(sock);

				fprintf(stdout, "sock_com = [%d]\n", sock_com);
				//handle communication beetween socket and web clients
				communication_handler(&g_env, idx, sock_com);
				exit(EXIT_SUCCESS);

				case -1:
					perror("fork");
					return (-1);

				//father
				default:
					close(sock_com);
					//Signal handler /* that's dirty */
					signal(SIGCHLD, SIG_IGN);
					break ;
			}
		}
		else
		{
			idx = choose_sock_idx(g_env, idx);
			g_env.com_tab[idx].sock = sock_com;
			
			//init child to parent pipes
			if (pipe(g_env.ctop_pipe[idx]) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			make_non_block(g_env.ctop_pipe[idx][0]);
			make_non_block(g_env.ctop_pipe[idx][1]);

			//init parent to child pipes
			if (pipe(g_env.ptoc_pipe[idx]) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			make_non_block(g_env.ptoc_pipe[idx][0]);
			make_non_block(g_env.ptoc_pipe[idx][1]);
			switch (fork())
			{
				//child
				case 0:
					close(sock);

					//child close ptoc input
					close(g_env.ptoc_pipe[idx][1]);

					//child close ctop output
					close(g_env.ctop_pipe[idx][0]);

					fprintf(stdout, "sock_com = [%d]\n", sock_com);

					//handle communication beetween socket and web clients
					communication_handler(&g_env, idx, sock_com);

					//send close message
					send_con_close(&g_env, idx);
					kill(getppid(), SIGRT_CLOSE);

					close(g_env.ctop_pipe[idx][0]);
					close(g_env.ctop_pipe[idx][1]);
					close(g_env.ptoc_pipe[idx][0]);
					close(g_env.ptoc_pipe[idx][1]);
					exit(EXIT_SUCCESS);

				case -1:
					perror("fork");
					return (-1);

				//father
				default:

					//keep ip
					get_ip(g_env.com_tab[idx].ip, sock_com);

					//father pipe com event handler
					check_deconnection(&g_env);

					// Update connection list
					update_con_list(&g_env);

					// Send connection list to children
					send_con_list(&g_env);

// 					usleep(1000);
					//Send ready event
					send_ws_ready(&g_env);
					//check if has challenger
					if (has_challenger(g_env))
						send_con_challenger(&g_env, 1);
					else
						send_con_challenger(&g_env, 0);


					close(sock_com);

					//father close ptoc input
					close(g_env.ptoc_pipe[idx][0]);

					//father close ctop output
					close(g_env.ctop_pipe[idx][1]);

					//Signal handler /* that's dirty */
					signal(SIGCHLD, SIG_IGN);
					signal(SIGRT_CLOSE, parent_signal_handler);
					signal(SIGRT_RCV, parent_signal_handler);
					break ;
			}
		}
		if (!full)
		{
			full = 1;
			usleep(3000);
			fprintf(stdout, "Host can't handle more than %d clients!\n",
					MAXPLAYER);
		}

		//father pipe com event handler
		if (check_deconnection(&g_env))
		{
// 			fprintf(stdout, "je me suis deconnecte !\n");
			// Update connection list
			update_con_list(&g_env);

			// Send connection list to children
			send_con_list(&g_env);
		}
	}
	return (0);
}
