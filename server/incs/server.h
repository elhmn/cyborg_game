#ifndef SERVER_H
# define SERVER_H

# include <wslay/wslay.h>
# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
# include <signal.h>

# define SIGRT_CLOSE	(SIGRTMIN)
# define MAXPLAYER		4

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
	int				ptoc_pipe[2];

	//child to parent pipe
	int				ctop_pipe[MAXPLAYER][2];

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

#endif
