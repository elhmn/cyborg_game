#ifndef SERVER_H
# define SERVER_H

# include <wslay/wslay.h>
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

# define SIGRT_CLOSE	(SIGRTMIN)
# define MAXPLAYER		4
# define BUFSIZE		1024

typedef struct		s_com
{
	int		sock;
	int		pid;
	char	ip[256];
}					t_com;

typedef struct		s_env
{
	t_com			com_tab[MAXPLAYER];

	//parent to child pipe
	int				ptoc_pipe[MAXPLAYER][2];

	//child to parent pipe
	int				ctop_pipe[MAXPLAYER][2];

	//this message is a tmp buffer
	char			msg[BUFSIZE];

	//this message must be sent to the lan
	char			lan_msg[BUFSIZE];

	//connection list
	char			con_list[BUFSIZE];
}					t_env;

/*
** wslay_callback.c
*/

int 				make_non_block(int fd);

ssize_t 			send_callback(wslay_event_context_ptr ctx,
					const uint8_t *data, size_t len, int flags,
					void *user_data);

ssize_t 			recv_callback(wslay_event_context_ptr ctx,
					uint8_t *buf, size_t len,
					int flags, void *user_data);

void				on_msg_recv_callback(wslay_event_context_ptr ctx,
					const struct wslay_event_on_msg_recv_arg *arg,
					void *user_data);

/*
** tools.c
*/

char**				str_split(char* a_str, const char a_delim);
char 				*cvtInt(char *str, int num);
void				free_tab(char ***tab);
void				show_tab(char **tab);

/*
** child_to_parent.c
*/

void				check_deconnection(t_env *env);
void				send_con_close(t_env *env, int idx);

/*
** parent_to_child.c
*/

void				update_con_list(t_env *env);
int					check_con_list(t_env *env, int idx);
void				send_con_list(t_env *env);

/*
** pipe.c
*/

void				pipe_com_write(int fd, const char *msg);
unsigned char		*pipe_com_read(int fd);

/*
** socket.c
*/


int					put_addr(struct sockaddr *addr, socklen_t len);
int					put_socket_ipv4(int sock);
int					create_socket_stream_ipv4(char *host_name,
					char *serv_name,
					int port,
					char *proto_name);

/*
** environement.c
*/

void				put_comtab(t_com *tab, int s);
void				init_comtab(t_com *tab, int s);

/*
** communication_handler.c
*/

int					communication_handler(t_env *env, int idx, int sock);

/*
** connection_handler.c
*/

int					connection_handler(int sock);

#endif
