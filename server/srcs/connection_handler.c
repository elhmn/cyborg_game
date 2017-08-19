#include "libft.h"
#include "server.h"
#include "websocket.h"

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

static int			isfull(t_env e)
{
	int		i;

	for (i = 0; i < MAXPLAYER; i++)
	{
		if (e.com_tab[i].sock == -1)
			return (0);
	}
	return (1);
}

int					connection_handler(int sock)
{
	t_env				env;
	int					sock_com;
	int					full;
	int					idx;

	socklen_t			len;
	struct sockaddr_in	addr;

	full = 0;
	idx = 0;
	len = sizeof(struct sockaddr_in);
	init_comtab(env.com_tab, MAXPLAYER);


	//inform kernel that we want to receive connection on that socket
	if (listen(sock, 5) < 0)
	{
		perror("listen");
		return (-1);
	}
	while (!quit_server())
	{
		if (!isfull(env))
		{
			put_comtab(env.com_tab, MAXPLAYER);//_DEBUG_//
			memset(&addr, 0, len);
			if ((sock_com = accept(sock, (struct sockaddr*)&addr, &len)) < 0)
			{
				perror("accept");
				return (-1);
			}
			idx = choose_sock_idx(env, idx);
			env.com_tab[idx].sock = sock_com;
			
			//init child to parent pipes
			if (pipe(env.ctop_pipe[idx]) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			make_non_block(env.ctop_pipe[idx][0]);
			make_non_block(env.ctop_pipe[idx][1]);

			//init parent to child pipes
			if (pipe(env.ptoc_pipe[idx]) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			make_non_block(env.ptoc_pipe[idx][0]);
			make_non_block(env.ptoc_pipe[idx][1]);
			switch (fork())
			{
				//child
				case 0:
					close(sock);

					//child close ptoc input
					close(env.ptoc_pipe[idx][1]);

					//child close ctop output
					close(env.ctop_pipe[idx][0]);

					fprintf(stdout, "sock_com = [%d]\n", sock_com);
					//handle communication beetween socket and web clients
					communication_handler(&env, idx, sock_com);

					//send close message
					send_con_close(&env, idx);

					close(env.ctop_pipe[idx][0]);
					close(env.ctop_pipe[idx][1]);
					close(env.ptoc_pipe[idx][0]);
					close(env.ptoc_pipe[idx][1]);
 					exit(EXIT_SUCCESS);

				case -1:
					perror("fork");
					return (-1);

				//father
 				default:

					//keep ip
					get_ip(env.com_tab[idx].ip, sock_com);

 					//father pipe com event handler
 					check_deconnection(&env);

					// Update connection list
					update_con_list(&env);

					// Send connection list to children
					send_con_list(&env);

 					close(sock_com);

					//father close ptoc input
					close(env.ptoc_pipe[idx][0]);

					//father close ctop output
					close(env.ctop_pipe[idx][1]);

	 				signal(SIGCHLD, SIG_IGN); /* that's dirty */


 					break ;
			}
		}
		else
		{
			if (!full)
			{
				full = 1;
				usleep(3000);
				fprintf(stdout, "Host can't handle more than %d clients!\n",
						MAXPLAYER);
			}
		}
		//father pipe com event handler
		check_deconnection(&env);
	}
	return (0);
}
