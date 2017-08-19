#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "libft.h"
#include "server.h"
#include "websocket.h"


int					put_socket_ipv4(int sock)
{
	struct sockaddr_in		addr;
	socklen_t				len;

	len = sizeof(struct sockaddr_in);
	if ((getsockname(sock,
					(struct sockaddr*)&addr, &len)) < 0)
	{
		perror("getsockname");
		return (-1);
	}
	fprintf(stdout, "IP = [%s] , PORT = [%u] \n",
			inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	return (0);
}

int					put_addr(struct sockaddr *addr, socklen_t len)
{
	char			hostbuf[NI_MAXHOST];
	char			servbuf[NI_MAXSERV];
	int				s;

	if ((s = getnameinfo(addr, len, hostbuf, NI_MAXHOST,
						servbuf, NI_MAXSERV, NI_NUMERICHOST
						| NI_NUMERICSERV)) != 0)
	{
		fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
		return (-1);
	}
	fprintf(stdout, "ip = [%s] , port = [%s] \n",
			hostbuf, servbuf);
	return (0);
}

int		create_socket_stream_ipv4(char *host_name,
									char *serv_name,
									int port,
									char *proto_name)
{
	int						sock;
	int						enable;
	struct sockaddr_in		addr;
	struct protoent			*protoent;
	struct servent			*servent;
	struct hostent			*hostent;

	enable = 1;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return (-1);
	}
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
			&enable, (socklen_t)sizeof(enable)) == -1)
	{
		perror("setsockopt");
		return (-1);
	}
	if (host_name && !(hostent = gethostbyname(host_name)))
	{
		perror("gethostbyname");
		return (-1);
	}
	if (proto_name && !(protoent = getprotobyname(proto_name)))
	{
		perror("getprotobyname");
		return (-1);
	}
	if (serv_name && !(servent = getservbyname(serv_name, protoent->p_name)))
	{
		perror("getservbyname");
		return (-1);
	}
	memset(&addr, 0, sizeof(struct addr_in*));
	addr.sin_family = AF_INET;
	addr.sin_port = (serv_name)
		? servent->s_port
		: htons(port);
	addr.sin_addr.s_addr = (host_name)
		? ((struct in_addr*)hostent->h_addr)->s_addr
		: htons(INADDR_ANY);
	if (bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind");
		return (-1);
	}
	return (sock);
}

# define BUFSIZE	1024

void				pipe_com_write(int fd, const char *msg)
{
	unsigned char	buf[BUFSIZE];

	if (msg && fd > 0)
	{
		strncpy((char*)buf, (const char*)msg, BUFSIZE);
// 		fprintf(stdout, "buf = [%s]\n", buf);//_DEBUG_//
		if (write(fd, buf, BUFSIZE) != BUFSIZE)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
}

unsigned char		*pipe_com_read(int fd)
{
	unsigned char	*buf;

	if (!(buf = (unsigned char*)malloc(sizeof(unsigned char) * BUFSIZE)))
	{
		fprintf(stderr, "Error : malloc");
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	memset(buf, 0, BUFSIZE);
	if (read(fd, buf, BUFSIZE) != BUFSIZE && errno != EAGAIN)
	{
		perror("read");
 		exit(EXIT_FAILURE);
	}
// 	fprintf(stdout, "buf = [%s]\n", buf);//_DEBUG_//
	return (buf);
}



int					communication_handler(t_env env, int idx, int sock)
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
			//send close message
// 			pipe_com_write(env.ctop_pipe[idx][1], "con");

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

//check whether the server must be closed or not
int					quit_server(void)
{
	return (0);
}


void				put_comtab(t_com *tab, int s)
{
	int		i;

	for (i = 0; i < s; i++)
	{
		fprintf(stdout, "(%d, %d)", tab[i].pid, tab[i].sock);
		if (i != s - 1)
		{
			fprintf(stdout, "; ");
		}
	}
	fprintf(stdout, "\n");
}

void				init_comtab(t_com *tab, int s)
{
	int		i;

	for (i = 0; i < s; i++)
	{
		tab[i].sock = -1;
		tab[i].pid = -1;
	}
}

// void				signal_handler(int signal)
// {
// 	if (signal == SIGRT_CLOSE)
// 	{
// 		fprintf(stdout, "signal close\n");
// 	}
// }
 
int					choose_sock_idx(t_env e, int idx)
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

int					isfull(t_env e)
{
	int		i;

	for (i = 0; i < MAXPLAYER; i++)
	{
		if (e.com_tab[i].sock == -1)
			return (0);
	}
	return (1);
}

void				get_ip(char *buf, int sock)
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

void				check_deconnection(t_env env)
{
	char				**tab;
	unsigned char		*buftmp;
	int					i;

	(void)env;
	i = 0;
	buftmp = NULL;
	//father pipe com event handler
	for (i = 0; i < MAXPLAYER; i++)
	{
// 			fprintf(stdout, "[%d]\n", i);
		if (env.com_tab[i].sock == -1)
			break ;
		buftmp = pipe_com_read(env.ctop_pipe[i][0]);
// 			fprintf(stdout, "je suis con\n");
		//parse message
		if (buftmp)
		{
// 				fprintf(stdout, "buftmp = [%s]\n", buftmp);
			tab = ft_strsplit((char*)buftmp, ':');
			if (tab)
			{
// 					show_tab(tab);
				if (!strcmp((const char*)tab[0], "con")
							&& !strcmp((const char*)tab[1], "close"))
				{
					env.com_tab[i].sock = -1;
					env.com_tab[i].pid = -1;
// 						fprintf(stdout, "Message get [%d]\n", i);
				}
				free(tab);
			}
			free(buftmp);
		}
	}
}

int					connection_handler(int sock)
{
	t_env				env;
	int					sock_com;
	int					full;
	int					i;
	int					idx;

	char				msg[BUFSIZE];
	socklen_t			len;
	struct sockaddr_in	addr;

	full = 0;
	idx = 0;
	len = sizeof(struct sockaddr_in);
	init_comtab(env.com_tab, MAXPLAYER);

	if (pipe(env.ptoc_pipe) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	make_non_block(env.ptoc_pipe[0]);
	make_non_block(env.ptoc_pipe[1]);
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
			if (pipe(env.ctop_pipe[idx]) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			make_non_block(env.ctop_pipe[idx][0]);
			make_non_block(env.ctop_pipe[idx][1]);
			switch (fork())
			{
				//child
				case 0:
					close(sock);

					//child close ptoc input
					close(env.ptoc_pipe[1]);

					//child close ctop output
					close(env.ctop_pipe[idx][0]);

					fprintf(stdout, "sock_com = [%d]\n", sock_com);
					//handle communication beetween socket and web clients
					communication_handler(env, idx, sock_com);

					//send close message
					memset(msg, 0, BUFSIZE);
					strcpy(msg, "con:close");
					pipe_com_write(env.ctop_pipe[idx][1], msg);

					fprintf(stdout, "closed ! index = [%d] \n", idx);//_DEBUG_//
					close(env.ctop_pipe[idx][0]);
					close(env.ctop_pipe[idx][1]);
 					exit(EXIT_SUCCESS);

				case -1:
					perror("fork");
					return (-1);

				//father
 				default:

					//keep ip
// 					get_ip(env.com_tab[idx].ip, sock_com);

// 					//father pipe com event handler
// 					check_deconnection(env);

 					close(sock_com);

					//father close ptoc input
					close(env.ptoc_pipe[0]);

					//father close ctop output
					close(env.ctop_pipe[idx][1]);
				
	 				signal(SIGCHLD, SIG_IGN); /* that's dirty */
	
					//send connection message
					memset(msg, 0, BUFSIZE);
					strcpy(msg, "con:open:");
					for (i = 0; i < MAXPLAYER; i++)
					{
						if (env.com_tab[i].sock == -1)
							continue;
						if (i != 0)
							strcat(msg, ";");
						strcat(msg, env.com_tab[i].ip);
					}
					put_comtab(env.com_tab, MAXPLAYER);//_DEBUG_//
					fprintf(stdout, "msg [%s]\n", msg);//_DEBUG_//
// 					pipe_com_write(env.ptoc_pipe[idx][1], msg);
 					break ;
			}
		}
		else
		{
			if (!full)
			{
				full = 1;
				usleep(3000);
				fprintf(stdout, "Host reached its limit and can't handle more than %d clients!\n", MAXPLAYER);
			}
		}
		//father pipe com event handler
		check_deconnection(env);
	}
	return (0);
}

int					main(int ac, char **av)
{
	int		sock_con;
	char	*port;
	
	sock_con = 0;
	port = "0";
	if (ac > 1)
		port = av[1];
	if ((sock_con = create_socket_stream_ipv4(NULL, NULL, atoi(port), "tcp")) < 0)
	{
		fprintf(stderr, "Error : sock_con < 0\n");
		exit(EXIT_FAILURE);
	}
	put_socket_ipv4(sock_con);//_DEBUG_//
	connection_handler(sock_con);
	close(sock_con);
	return (0);
}
