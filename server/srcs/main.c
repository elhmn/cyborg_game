#include "libft.h"
#include "server.h"
#include "websocket.h"


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
