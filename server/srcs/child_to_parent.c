#include "libft.h"
#include "server.h"
#include "websocket.h"

void				check_deconnection(t_env *env)
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
// 		fprintf(stdout, "[%d]\n", i);
		if (env->com_tab[i].sock == -1)
			continue ;
		buftmp = pipe_com_read(env->ctop_pipe[i][0]);
// 		fprintf(stdout, "je suis con\n");
		//parse message
		if (buftmp)
		{
//  			fprintf(stdout, "buftmp = [%s]\n", buftmp);
			tab = ft_strsplit((char*)buftmp, ':');
			if (tab)
			{
// 				show_tab(tab);
				if (!strcmp((const char*)tab[0], "con")
							&& !strcmp((const char*)tab[1], "close"))
				{
					env->com_tab[i].sock = -1;
					env->com_tab[i].pid = -1;
// 					fprintf(stdout, "Message get [%d]\n", i);
				}
				free_tab(&tab);
			}
			free(buftmp);
		}
	}
}

/*
** Send socket connection close event
*/

void		send_con_close(t_env *env, int idx)
{
	if (env)
	{
		memset(env->msg, 0, BUFSIZE);
		strcpy(env->msg, "con:close");
		pipe_com_write(env->ctop_pipe[idx][1], env->msg);

		fprintf(stdout, "closed ! index = [%d] \n", idx);//_DEBUG_//
	}
}
