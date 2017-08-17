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
#include <signal.h>
#include "server.h"

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
	fprintf(stdout, "IP = [%s] , PORT = [%s] \n",
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

int		communication_handler(int sock)
{
	socklen_t				len;
	struct sockaddr_in		addr;

	len = sizeof(struct sockaddr_in);
	if (getpeername(sock, (struct sockaddr*)&addr, &len) < 0)
	{
		perror("getpeername");
		return (-1);
	}
 	fprintf(stdout, "Peer : ");
	put_addr((struct sockaddr*)&addr, len);
	close(sock);
	return (0);
}

//check whether the server must be closed or not
int					quit_server(void)
{
	return (0);
}

int		connection_handler(int sock)
{
	int					sock_com;
	socklen_t			len;
	struct sockaddr_in	addr;

	len = sizeof(struct sockaddr_in);
	//inform kernel that we want to receive connection on that socket
	if (listen(sock, 5) < 0)
	{
		perror("listen");
		return (-1);
	}
	while (!quit_server())
	{
		memset(&addr, 0, len);
		if ((sock_com = accept(sock, (struct sockaddr*)&addr, &len)) < 0)
		{
			perror("accept");
			return (-1);
		}
		switch (fork())
		{
			//child
			case 0:
				close(sock);
				communication_handler(sock_com);
				exit(EXIT_SUCCESS);
			case -1:
				perror("fork");
				return (-1);
			//father
			default:
				close(sock_com);
// 				signal(SIGCHLD, SIG_IGN); /* that's dirty */
				signal(SIGCHLD, SIG_DFL); /* that's dirty */
		}
	}
	return (0);
}

int		main(int ac, char **av)
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
