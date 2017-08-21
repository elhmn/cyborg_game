#include "libft.h"
#include "server.h"
#include "websocket.h"

int				check_deconnection(t_env *env)
{
	char				**tab;
	unsigned char		*buftmp;
	int					i;
	int					ret;

	(void)env;
	i = 0;
	ret = 0;
	buftmp = NULL;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
	//father pipe com event handler
	for (i = 0; i < MAXPLAYER; i++)
	{
// 		fprintf(stdout, "[%d]\n", i);
//  		fprintf(stdout, "buftmp = [%s]\n", buftmp);
		if (env->com_tab[i].sock == -1)
			continue ;
		buftmp = pipe_com_read(env->ctop_pipe[i][0]);
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
					env->ready[i] = 0;
// 					fprintf(stdout, "Message get [%d]\n", i);
					ret = 1;
				}
				free_tab(&tab);
			}
			free(buftmp);
		}
	}
	return (ret);
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

/*
** send children received message 
*/

void		send_rcv_msg(t_env *env, int idx, char *msg)
{
	if (env)
	{
		memset(env->msg, 0, BUFSIZE);
		strcpy(env->msg, msg);
		pipe_com_write(env->ctop_pipe[idx][1], env->msg);
		fprintf(stdout, "reveived message ! from index = [%d] :: [%s]\n", idx, env->msg);//_DEBUG_//
	}
}

int				check_rcv_msg(t_env *env)
{
	char				**tab;
	unsigned char		*buftmp;
	int					i;
	int					idx;
	int					ret;

	(void)env;
	i = 0;
	ret = 0;
	buftmp = NULL;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
	//father pipe com event handler
	for (i = 0; i < MAXPLAYER; i++)
	{
// 		fprintf(stdout, "[%d]\n", i);
//  		fprintf(stdout, "buftmp = [%s]\n", buftmp);
		if (env->com_tab[i].sock == -1)
			continue ;
		buftmp = pipe_com_read(env->ctop_pipe[i][0]);
 		fprintf(stdout, "check_rcv_msg buftmp = [%s]\n", buftmp);
		//parse message
		if (buftmp)
		{
			tab = ft_strsplit((char*)buftmp, '/');
			if (tab)
			{
// 				show_tab(tab);
				idx = atoi(tab[0]);
				if (idx >= 0
					&& idx <= MAXPLAYER
					&& !strcmp((const char*)tab[1], "ws"))
				{
					env->rcv_idx = i;
					strcpy(env->rcv_msg[i], (const char*)buftmp);
					fprintf(stdout, "Message get [%d]\n", i);
					ret = 1;
				}
				free_tab(&tab);
			}
			free(buftmp);
		}
	}
	return (ret);
}

/*
** client rcv data parsing and interaction
*/

int			update_ready_tab(t_env *env, char **tab)
{
	int		ret;
	int		idx;

	ret = 0;
	fprintf(stdout, "update ready tab \n");
	if (env && tab)
	{
		idx = atoi(tab[0]);
		if (idx >= 0
			&& idx <= MAXPLAYER
			&& !strcmp((const char*)tab[1], "ws")
			&& !strcmp((const char*)tab[2], "ready"))
		{
// 			env->rcv_idx = i;
			env->ready[idx] = 1;
			fprintf(stdout, "Message get [%d]\n", idx);
			ret = 1;
		}
	}
	return (ret);
}
