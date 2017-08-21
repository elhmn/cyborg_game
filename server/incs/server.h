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
#include <time.h>

//cyborg limit min
# define MIN			0

//cyborg limit max
# define MAX			1000

/*
** echo message coming from clients
*/

# define ECHO			"hello"

/*
** Realtime signal definition
*/

# define SIGRT_CLOSE	(SIGRTMIN)

// If child received message
# define SIGRT_RCV		(SIGRTMIN + 1)

// If room has challenger
# define SIGRT_CHAL		(SIGRTMIN + 2)

// If room no more has challenger
# define SIGRT_NOCHAL	(SIGRTMIN + 3)

# ifndef MAXPLAYER
	#  define MAXPLAYER		4
# endif

# define BUFSIZE		1024

typedef struct		s_com
{
	int		sock;
	int		pid;
	char	ip[256];
}					t_com;

enum				e_state
{
	WAITROOM,
	START,
	CHOOSE,
	COMPARE,
	RESULT,
	EDITROLE,
	END
};

typedef struct		s_env
{
	t_com			com_tab[MAXPLAYER];

	//parent children pids
	int				chpid[MAXPLAYER];

	//child index
	int				chidx;

	//room full
	int				isfull;

	//player ready
	int				ready[MAXPLAYER];

	//cyborg choice
	int				number;

	//cyborg limit min
	int				min;

	//cyborg limit max
	int				max;

	//game state	e_state
	int				state;

	//parent to child pipe
	int				ptoc_pipe[MAXPLAYER][2];

	//child to parent pipe
	int				ctop_pipe[MAXPLAYER][2];

	//this message is a tmp buffer
	char			msg[BUFSIZE];

	//this message must be sent to the lan
	char			lan_msg[MAXPLAYER][BUFSIZE];

	//received message buffer
	char			rcv_msg[MAXPLAYER][BUFSIZE];

	//child msg reveived index
	int				rcv_idx;

	//connection list
	char			con_list[BUFSIZE];

	int				ctop_update;
	int				ptoc_update;
}					t_env;

/*
** fstate used in communication_handler.c
*/

typedef void				(*fstate_c)(wslay_event_context_ptr ctx,
										t_env *env, int idx);

typedef void				(*fstate_p)(t_env *env);


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
void				init_tab(int *tab, int sz, int val);

/*
** child_to_parent.c
*/

int					check_deconnection(t_env *env);
int					check_rcv_msg(t_env *env);
void				send_con_close(t_env *env, int idx);
void				send_rcv_msg(t_env *env, int idx, char *msg);
int					update_ready_tab(t_env *env, char **tab);
int					update_ws_start(t_env *env, char **tab);

/*
** child_to_client.c
*/

void				send_text_msg(wslay_event_context_ptr ctx, char *msg);

/*
** parent_to_child.c
*/

void				update_con_list(t_env *env);
void				send_con_list(t_env *env);
int					check_con_list(t_env *env, int idx, unsigned char *buftmp);

void				send_con_challenger(t_env *env, int state);
int					check_con_challenger(t_env *env, int idx, unsigned char *buftmp);
void				send_ws_ready(t_env *env);
int					check_ws_ready(t_env *env, int idx, unsigned char *buftmp);

void				send_ws_allready(t_env *env);
int					check_ws_allready(t_env *env, int idx, unsigned char *buftmp);
void				send_ws_start(t_env *env);
int					check_ws_start(t_env *env, int idx, unsigned char *buftmp);

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

void				put_comtab(t_com *tab, int sz);
void				init_comtab(t_com *tab, int sz);
void				init_env(t_env *env);

//Check whether the was a parent to child communication or not
int					isptoc(t_env *env);

//Check whether the was a child to parent communication or not
int					isctop(t_env *env);

void				set_ptoc(t_env *env);
void				set_ctop(t_env *env);

/*
** communication_handler.c
*/

int					communication_handler(t_env *env, int idx, int sock);

/*
** connection_handler.c
*/

int					connection_handler(int sock);

#endif
